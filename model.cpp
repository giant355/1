#include "model.h"
#include<fstream>
#include<sstream>
#include<string>

Model::Model(const std::string filename) {
	std::ifstream in;
	in.open(filename);
	if (!in.is_open()) {
		std::cerr << "can't open " << filename;
		return;
	}
	std::string line;
	std::istringstream iss;
	char trash;
	while (std::getline(in,line)) {
		iss.clear();
		iss.str(line);
		if (!line.compare(0,2,"v ")) {
			iss >> trash;
			vec4 vec{0,0,0,1};
			for (int i : {0, 1, 2}) iss >> vec[i];
			verts.push_back(vec);
		}
		else if (!line.compare(0, 3, "vt ")) {
			iss >> trash >> trash;
			vec2 uv;
			for (int i : {0, 1})iss >> uv[i];
			tex.push_back({uv.x,1-uv.y});
		}
		else if (!line.compare(0,3,"vn ")){
			iss >> trash >> trash;
			vec4 vec;
			for (int i : {0, 1, 2})iss >> vec[i];
			norms.push_back(vec);
		}
		else if (!line.compare(0,2,"f ")) {
			iss >> trash;
			int v, n, t, cnt =0;
			while (iss >> v >> trash >> t >> trash >> n) {
				facet_vrt.push_back(--v);
				facet_tex.push_back(--t);
				facet_nrm.push_back(--n);
				cnt++;
			}
			if (3 != cnt) {
				std::cerr << "Error: the obj file is supposed to be triangulated" << std::endl;
				return;
			}
		}
	}
	auto load_texture = [&filename](const std::string suffix, TGAImage& img) {
		size_t dot = filename.find_last_of(".");
		if (dot == std::string::npos) return;
		std::string texfile = filename.substr(0, dot) + suffix;
		std::cerr << "texture file " << texfile << " loading " << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
		};
	load_texture("_nm.tga", normalmap);
	in.close();
}
int Model::nverts() const {
	return static_cast<int>(verts.size());
}
int Model::nfaces() const {
	return static_cast<int>(facet_vrt.size() / 3);
}
vec4 Model::vert(const int iface, const int nthvert) const {
	int index = facet_vrt[iface * 3 + nthvert];
	return verts[index];
}
vec4 Model::vert(const int i)const {
	return verts[i];
}
vec4 Model::normal(const int iface, const int nthvert) const {
    int index = facet_nrm[iface * 3 + nthvert];
    return norms[index];
}
vec4 Model::normal(const vec2& uv) const {
	TGAColor c = normalmap.get(uv[0] * normalmap.width(), uv[1] * normalmap.height());
	return vec4{ (double)c[2],(double)c[1],(double)c[0],0 }*2. / 255. - vec4{ 1,1,1,0 };
}
vec2 Model::uv(const int iface, const int nthvert) const {
	int index = facet_tex[iface * 3 + nthvert];
	return tex[index];
}

