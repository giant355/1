#include "my_gl.h"
#include "model.h"
#include "geometry.h"
#include <cmath>


extern mat<4, 4> ModelView, Perspective;
extern std::vector<double> zbuffer;


struct ShaderProgram1 : Shader {
    TGAColor color{};
    vec4 vertex(const vec4& v) const override {
        return v *ModelTrans* ModelView * Perspective;
    }
    std::pair<bool, TGAColor> fragment(const vec3 bar)const override {
        return { false,color };
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
    constexpr int width = 800;      // output image size
    constexpr int height = 800;
    constexpr vec3    eye{ -1, 0, 2 }; // camera position
    constexpr vec3 center{ 0, 0, 0 }; // camera direction
    constexpr vec3     up{ 0, 1, 0 }; // camera up vector

    lookAt(eye,center,up);
    init_perspective(norm(eye-center));
    init_viewport(0, 0, width, height);
    init_zbuffer(width, height);
    TGAImage framebuffer(width, height, TGAImage::RGB,{ 177, 195, 209, 255 });

    for (int m = 1; m < argc; m++) {
        Model model(argv[m]);
        ShaderProgram1 shader;
        shader.ModelTrans = rotation({1,1,1},3.14/6);
        for (int f = 0; f < model.nfaces(); f++) {
            shader.color = { static_cast<unsigned char>(std::rand() % 255),static_cast<unsigned char>(std::rand() % 255), static_cast<unsigned char>(std::rand() % 255), 255 };
            Triangle clip = { shader.vertex(model.vert(f, 0)) ,shader.vertex(model.vert(f, 1)) ,shader.vertex(model.vert(f, 2)) };
            rasterize(clip, shader, framebuffer);
        }
    }

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
    
}