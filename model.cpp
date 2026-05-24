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
		if (line[0] == 'v' && line[1] == ' ') {
			iss >> trash;
			vec4 vec{0,0,0,1};
			for (int i : {0, 1, 2}) iss >> vec[i];
			verts.push_back(vec);
		}
		else if (line[0] == 'v' && line[1] == 'n')
		{
			iss >> trash >> trash;
			vec4 vec;
			for (int i : {0, 1, 2})iss >> vec[i];
			norms.push_back(vec);
		}
		else if (line[0] == 'f') {
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
vec4 Model::normal(const int i) const {
    return norms[i];
}
