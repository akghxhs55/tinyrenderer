#include <unistd.h>

#include "tgaimage.h"
#include "model.h"


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


Vec3f _isInTriangle(Vec2i *v, Vec2i P) {
    Vec3f t =   Vec3f(v[1].x - v[0].x, v[2].x - v[0].x, v[0].x - P.x)
              ^ Vec3f(v[1].y - v[0].y, v[2].y - v[0].y, v[0].y - P.y);
    return Vec3f(t.z - t.x - t.y, t.x, t.y) / t.z;
}


void setTriangle(Vec2i *v, TGAImage &image, TGAColor color) {
    Vec2i bboxMin(image.get_width() - 1,  image.get_height() - 1);
    Vec2i bboxMax(0, 0);
    Vec2i clamp(image.get_width() - 1,  image.get_height() - 1);
    for (int i = 0; i < 3; i++) {
        bboxMin.x = std::max(0, std::min(bboxMin.x, v[i].x));
        bboxMin.y = std::max(0, std::min(bboxMin.y, v[i].y));
        bboxMax.x = std::min(clamp.x, std::max(bboxMax.x, v[i].x));
        bboxMax.y = std::min(clamp.y, std::max(bboxMax.y, v[i].y));
    }
    
    Vec2i P;
    for (P.x = bboxMin.x; P.x <= bboxMax.x; P.x++) {
        for (P.y = bboxMin.y; P.y <= bboxMax.y; P.y++) {
            Vec3f isTr = _isInTriangle(v, P);
            if (isTr.x < 0 || isTr.y < 0 || isTr.z < 0 || isTr.x > 1 || isTr.y > 1 || isTr.z > 1) {
                continue;
            }
            image.set(P.x, P.y, color);
        }
    }
}


const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor   red = TGAColor(255,   0,   0, 255);
const TGAColor green = TGAColor(  0, 255,   0, 255);
const TGAColor  blue = TGAColor(  0,   0, 255, 255);


int main(int argc, char** argv) {
    TGAImage image(1000, 1000, TGAImage::RGBA);

    Model model("obj/african_head.obj");
    Vec3f light_dir(0, 0, -1);

    for (int i = 0; i < model.nfaces(); i++) {
        std::vector<int> face = model.face(i);
        Vec2i screenCoords[3];
        Vec3f worldCoords[3];
        for (int j = 0; j < 3; j++) {
            Vec3f v = model.vert(face[j]);
            screenCoords[j] = Vec2i((v.x + 1.0) * image.get_width() / 2.0, (v.y + 1.0) * image.get_height() / 2.0);
            worldCoords[j] = v;
        }
        Vec3f n = (worldCoords[2] - worldCoords[0]) ^ (worldCoords[1] - worldCoords[0]);
        n.normalize();
        float intensity = n * light_dir;
        if (intensity > 0) {
            setTriangle(screenCoords, image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
        }
    }

    image.flip_vertically();
    image.write_tga_file("output/lesson2-04.tga");
    return 0;
}
