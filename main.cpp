#include <unistd.h>

#include "tgaimage.h"
#include "model.h"

#include <iostream>


Vec3f barycentric(Vec3f *v, Vec3f P) {
    Vec3f ab = v[1] - v[0];
    Vec3f ac = v[2] - v[0];
    Vec3f pa = v[0] - P;
    Vec3f t = Vec3f(ab.x, ac.x, pa.x) ^ Vec3f(ab.y, ac.y, pa.y);
    t = t / t.z;
    return Vec3f(1 - t.x - t.y, t.x, t.y);
}


void setLine(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    bool transposed = false;
    if (abs(x0-x1) < abs(y0-y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        transposed = true;
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    if (x0 == x1) {
        image.set(x0, y0, color);
        return;
    }

    int dx = x1 - x0;
    int dy = y1 - y0;
    int derror = std::abs(dy)*2;
    int error = 0;
    int y = y0;
    for (int x = x0; x <= x1; x++) {
        if (transposed) {
            image.set(y, x, color);
        }
        else {
            image.set(x, y, color);
        }
        error += derror;
        if (error > dx) {
            y += (y1 > y0 ? 1 : -1);
            error -= dx*2;
        }
    }
}


void setTriangle(Vec3f *v, float *zBuffer, TGAImage &image, TGAImage &diffusemap, Vec2f *uv, float intensity) {
    Vec2f bboxMin( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max());
    Vec2f bboxMax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    Vec2f clamp(image.get_width() - 1, image.get_height() - 1);
    for (int i = 0; i < 3; i++) {
        bboxMin.x = std::max(0.0f, std::min(bboxMin.x, v[i].x));
        bboxMax.x = std::min(clamp.x, std::max(bboxMax.x, v[i].x));
        bboxMin.y = std::max(0.0f, std::min(bboxMin.y, v[i].y));
        bboxMax.y = std::min(clamp.y, std::max(bboxMax.y, v[i].y));
    }

    Vec3f P;
    for (P.x = bboxMin.x; P.x <= bboxMax.x; P.x++) {
        for (P.y = bboxMin.y; P.y <= bboxMax.y; P.y++) {
            Vec3f bcCoord = barycentric(v, P);
            if (bcCoord.x < 0 || bcCoord.y < 0 || bcCoord.z < 0) continue;
            P.z = bcCoord * Vec3f(v[0].z, v[1].z, v[2].z);
            if (zBuffer[int(P.x + P.y * image.get_width())] < P.z) {
                zBuffer[int(P.x + P.y * image.get_width())] = P.z;
                Vec2f uvCoord = uv[0] * bcCoord.x + uv[1] * bcCoord.y + uv[2] * bcCoord.z;
                TGAColor color = diffusemap.get(uvCoord.x * diffusemap.get_width(), uvCoord.y * diffusemap.get_height());
                image.set(P.x, P.y, color * intensity);
            }
        }
    }
}


const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor   red = TGAColor(255,   0,   0, 255);
const TGAColor green = TGAColor(  0, 255,   0, 255);
const TGAColor  blue = TGAColor(  0,   0, 255, 255);


int main(int argc, char** argv) {
    TGAImage image(1024, 1024, TGAImage::RGB);

    Model model("obj/african_head.obj");
    model.load_diffuse("obj/african_head_diffuse.tga");
    Vec3f light_dir(0, 0, -1);
    float *zBuffer = new float[image.get_width() * image.get_height()];
    for (int i = image.get_width() * image.get_height() - 1; i >= 0; i--) zBuffer[i] = -std::numeric_limits<float>::max();
    for (int i = 0; i < model.nfaces(); i++) {
        std::vector<int> face = model.face(i);
        std::vector<int> uvface = model.uvface(i);
        Vec3f screenCoords[3];
        Vec3f worldCoords[3];
        Vec2f uvCoords[3];
        for (int j = 0; j < 3; j++) {
            Vec3f v = model.vert(face[j]);
            screenCoords[j] = Vec3f(int((v.x + 1) * image.get_width() / 2 + 0.5), int((v.y + 1) * image.get_height() / 2 + 0.5), v.z);
            worldCoords[j] = v;
            uvCoords[j] = model.uv(uvface[j]);
        }

        Vec3f n = (worldCoords[2] - worldCoords[0]) ^ (worldCoords[1] - worldCoords[0]);
        n.normalize();
        float intensity = n * light_dir;
        if (intensity > 0) {
            setTriangle(screenCoords, zBuffer, image, model.diffusemap(), uvCoords, intensity);
        }
    }

    image.flip_vertically();
    image.write_tga_file("output/lesson3-02.tga");
    return 0;
}
