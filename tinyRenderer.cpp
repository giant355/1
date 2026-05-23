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
    //vec3 tri[3];
    Triangle tri;
    vec3 l;
    Blinn_PhongShader(const vec3& lightDir) {
        l=normalized((vec4{lightDir.x,lightDir.y,lightDir.z,0}*ModelTrans*ModelView).xyz());
    }
    vec4 vertex(const vec4& v) const override {
        vec4 gl_Position = v *ModelTrans * ModelView * Perspective;
        return gl_Position;
    }
    std::pair<bool, TGAColor> fragment(const vec3 bar)const override {
        TGAColor gl_FragColor{255,255,255,255};
        vec3 temp1 = tri[1].xyz() - tri[0].xyz();
        vec3 temp2 = tri[2].xyz() - tri[1].xyz();
        vec3 n=normalized(cross(temp1, temp2));//这里假设事先知道缠绕方向

        double ambient = .3;
        double diff = std::max(0., n * l);
        vec3 coordinate =
         mat<3,3>{ {{ tri[0].x,tri[0].y,tri[0].z },
                    { tri[1].x,tri[1].y,tri[1].z },
                    { tri[2].x,tri[2].y,tri[2].z }} }*bar;
        vec3 v = eye - coordinate;
        vec3 half = normalized(l + v);
        double spec = std::pow(std::max(half * n, 0.),35);
        double result = ambient + diff + spec;
        for (int channel : {0, 1, 2})
            gl_FragColor[channel] *= std::min(1., ambient + .4 * diff + .9 * spec);
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
    init_viewport(0, 0, width, height);
    init_zbuffer(width, height);
    TGAImage framebuffer(width, height, TGAImage::RGB);

    for (int m = 1; m < argc; m++) {
        Model model(argv[m]);
        Blinn_PhongShader shader({ 1, 1, 1 });
        for (int f = 0; f < model.nfaces(); f++) {
            shader.tri[0] = shader.vertex(model.vert(f, 0));
            shader.tri[1] = shader.vertex(model.vert(f, 1));
            shader.tri[2] = shader.vertex(model.vert(f, 2));
            rasterize(shader.tri,shader,framebuffer);
        }
    }

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
    
}