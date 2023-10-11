#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"
#include "tgaimage.h"

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<std::vector<int> > faces_;
    std::vector<std::vector<int> > uvfaces_;
    std::vector<Vec2f> uvs_;
    TGAImage diffusemap_;
public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
    int nuvs();
	Vec3f vert(int i);
	std::vector<int> face(int idx);
    std::vector<int> uvface(int idx);
    Vec2f uv(int i);
    void load_diffuse(const char *filename);
    TGAImage &diffusemap();
};

#endif //__MODEL_H__
