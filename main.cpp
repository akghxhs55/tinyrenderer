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


const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor   red = TGAColor(255,   0,   0, 255);


int main(int argc, char** argv) {
    TGAImage image(1000, 1000, TGAImage::RGBA);
    
    Model model("obj/african_head.obj");
    for (int i = 0; i < model.nfaces(); i++) {
        std::vector<int> face = model.face(i);
        for (int j = 0; j < 3; j++) {
            Vec3f v0 = model.vert(face[j]);
            Vec3f v1 = model.vert(face[(j+1)%3]);
            int x0 = (v0.x+1.0) * image.get_width()/2.0;
            int y0 = (v0.y+1.0) * image.get_height()/2.0;
            int x1 = (v1.x+1.0) * image.get_width()/2.0;
            int y1 = (v1.y+1.0) * image.get_height()/2.0;
            setLine(x0, y0, x1, y1, image, white);
        }
    }

    image.flip_vertically();
    image.write_tga_file("output/lesson1-02.tga");
    return 0;
}
