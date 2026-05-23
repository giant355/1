#include "my_gl.h"
#include "model.h"
#include "geometry.h"
#include <cmath>


extern mat<4, 4> ModelView, Perspective;
extern std::vector<double> zbuffer;

constexpr int width = 800;      // output image size
constexpr int height = 800;
constexpr vec3    eye{ -1, 0, 2 }; // camera position
constexpr vec3 center{ 0, 0, 0 }; // camera direction
constexpr vec3     up{ 0, 1, 0 }; // camera up vector

struct Blinn_PhongShader : Shader {
    TGAColor color{};
    Triangle tri; //观察空间
    Model model;
    vec3 l;
    Blinn_PhongShader(const vec3& lightDir,const Model& m):model(m) {
        l = normalized((ModelView * (ModelTrans * vec4{lightDir.x,lightDir.y,lightDir.z,0})).xyz());//观察空间
    }
    virtual vec4 vertex(const int face,const int vert) {
        vec4 v = (ModelView*(ModelTrans*model.vert(face, vert)));//观察空间
        tri[vert] = v;
        return Perspective*v ;//裁剪空间
    }
    std::pair<bool, TGAColor> fragment(const vec3 bar)const override {
        TGAColor gl_FragColor{255,255,255,255};
        vec3 temp1 = tri[1].xyz() - tri[0].xyz();
        vec3 temp2 = tri[2].xyz() - tri[1].xyz();
        vec3 n=normalized(cross(temp1, temp2));//这里假设事先知道缠绕方向

        double ambient = .1;
        double diff = std::max(0., n * l);
        vec3 r = 2 * n * (n * l) - l;
        double spec = std::pow(std::max(r.z, 0.), 35);
        double lighting = ambient + .4 * diff + .9 * spec;
        for (int channel : {0, 1, 2})
            gl_FragColor[channel] *= std::min(1., lighting);
        return { false, gl_FragColor };
    }
};
mat<4, 4> rotation(const vec3& axis, double angle) {
    vec3 k = normalized(axis);
    double c = cos(angle), s = sin(angle);
    double t = 1 - c;

    return { {{ k.x * k.x * t + c,     k.x * k.y * t - k.z * s, k.x * k.z * t + k.y * s, 0 },
             { k.y * k.x * t + k.z * s, k.y * k.y * t + c,     k.y * k.z * t - k.x * s, 0 },
             { k.z * k.x * t - k.y * s, k.z * k.y * t + k.x * s, k.z * k.z * t + c,     0 },
             { 0,                 0,                 0,                 1 }} };
}
int main(int argc, char** argv) {
    lookAt(eye,center,up);
    init_perspective(norm(eye-center));
    init_viewport(width / 16, height / 16, width * 7 / 8, height * 7 / 8);
    init_zbuffer(width, height);
    TGAImage framebuffer(width, height, TGAImage::RGB);

    for (int m = 1; m < argc; m++) {
        Model model(argv[m]);
        Blinn_PhongShader shader({ 1, 1, 1 }, model);
        //shader.ModelTrans=shader.ModelTrans * rotation({ 0, 1, 0 }, -3.14/6);
        for (int i = 0; i < model.nfaces(); i++) {
            Triangle clip = {shader.vertex(i, 0), shader.vertex(i, 1), shader.vertex(i, 2)};
            rasterize(clip, shader, framebuffer);
        }
    }

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
    
}