#include "my_gl.h"
#include "model.h"
#include "geometry.h"


extern mat<4, 4> ModelView, Perspective;
extern std::vector<double> zbuffer;

struct ShaderProgram1 : Shader {
    TGAColor color{};
    void vertex(vec4 v) const override {
        v = v * ModelView * Perspective;
    }
    std::pair<bool, TGAColor> fragment(const vec3 bar)const override {
        return { false,color };
    }
};
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
        vertexProcessing(shader, model);
        for (int f = 0; f < model.nfaces(); f++) {
            shader.color = { static_cast<unsigned char>(std::rand() % 255),static_cast<unsigned char>(std::rand() % 255), static_cast<unsigned char>(std::rand() % 255), 255 };
            Triangle clip = { model.vert(f,0),model.vert(f,1) ,model.vert(f,2) };
            rasterize(clip, shader, framebuffer);
        }
    }

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
    
}