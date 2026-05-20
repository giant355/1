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
		if (line[0] == 'v') {
			iss >> trash;
			vec4 vec{0,0,0,1};
			for (int i : {0, 1, 2}) iss >> vec[i];
			verts.push_back(vec);
		}
		else if (line[0] == 'f') {
			iss >> trash;
			int idx0 = 0, idx1 = 0, idx2 = 0;
			std::string s0,s1,s2;
			iss >> s0>>s1>>s2;
			idx0 = std::stoi(s0.substr(0, s0.find('/'))) - 1;
			idx1 = std::stoi(s1.substr(0, s1.find('/'))) - 1;
			idx2 = std::stoi(s2.substr(0, s2.find('/'))) - 1;
			facet_vrt.push_back(idx0);
			facet_vrt.push_back(idx1);
			facet_vrt.push_back(idx2);
		}
	}
	in.close();
}
vec4 Model::vert(const int i)const {
	return verts[i];
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