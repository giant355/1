#pragma once
#include <vector>
#include "geometry.h"

class Model {
    std::vector<vec4> verts = {};    // array of vertices        ©´ generally speaking, these arrays
    std::vector<vec4> norms = {};    // array of normal vectors  ©¦ do not have the same size
    std::vector<vec2> tex = {};      // array of tex coords      ©¼ check the logs of the Model() constructor
    std::vector<int> facet_vrt = {}; // per-triangle index in the above array
    std::vector<int> facet_nrm = {};
    std::vector<int> facet_tex = {};
public:
    Model(const std::string filename);
    int nverts() const; // number of vertices
    int nfaces() const; // number of triangles
    vec4 vert(const int i) const;                          // 0 <= i < nverts()
    vec4 normal(const int iface, const int nthvert) const;
    vec4 normal(const int i)const;
    vec4 vert(const int iface, const int nthvert) const;   // 0 <= iface <= nfaces(), 0 <= nthvert < 3
};