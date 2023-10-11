#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : verts_(), faces_() {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v.raw[i];
            verts_.push_back(v);
        } else if (!line.compare(0, 2, "f ")) {
            std::vector<int> f;
            std::vector<int> t;
            int itrash, vertex, texcoord;
            iss >> trash;
            while (iss >> vertex >> trash >> texcoord >> trash >> itrash) {
                f.push_back(--vertex);
                t.push_back(--texcoord);
            }
            faces_.push_back(f);
            uvfaces_.push_back(t);
        } else if (!line.compare(0, 3, "vt ")) {
            iss >> trash >> trash;
            Vec2f vt;
            for (int i=0;i<2;i++) iss >> vt.raw[i];
            uvs_.push_back(vt);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << " uv# " << uvs_.size() << std::endl;
}

Model::~Model() {
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

int Model::nuvs() {
    return (int)uvs_.size();
}

std::vector<int> Model::face(int idx) {
    return faces_[idx];
}

std::vector<int> Model::uvface(int idx) {
    return uvfaces_[idx];
}

Vec3f Model::vert(int i) {
    return verts_[i];
}

Vec2f Model::uv(int i) {
    return uvs_[i];
}

void Model::load_diffuse(const char *filename) {
    diffusemap_.read_tga_file(filename);
    diffusemap_.flip_vertically();
}

TGAImage &Model::diffusemap() {
    return diffusemap_;
}
