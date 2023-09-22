#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor   red = TGAColor(255,  50,  50, 255);

int main(int argc, char** argv) {
    TGAImage image(100, 100, TGAImage::RGBA);
    image.set(52, 41, red);
    image.flip_vertically();
    image.write_tga_file("output/output.tga");
    return 0;
}
