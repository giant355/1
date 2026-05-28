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
    Model model;
    vec3 l;
    vec2 uv[3];
    vec3 tri[3];
    vec4 triN[3];
    Blinn_PhongShader(const vec3& lightDir,const Model& m):model(m) {
        l = normalized((ModelView * (ModelTrans * vec4{lightDir.x,lightDir.y,lightDir.z,0})).xyz());//观察空间
    }
    virtual vec4 vertex(const int face,const int vert) {
        vec4 v = (ModelView*(ModelTrans*model.vert(face, vert)));//观察空间

        tri[vert] = v.xyz();
        triN[vert] = model.normal(face,vert);
        uv[vert]  = model.uv(face, vert);

        return Perspective*v ;//裁剪空间
    }
    std::pair<bool, TGAColor> fragment(const vec3 bar)const override {
        vec2 barUV = uv[0] * bar.x + uv[1] * bar.y + uv[2] * bar.z;
        vec4 barN = triN[0] * bar.x + triN[1] * bar.y + triN[2] * bar.z;
        vec3 N = normalized((ModelView * ModelTrans.invert_transpose() * barN).xyz());

        mat<2,2> U{ {{uv[1] - uv[0]},
                     {uv[2] - uv[0]}} };
        mat<2,3> E{{{tri[1]- tri[0]},
                    {tri[2]- tri[0]}} };
        mat<2, 3>TB = U.invert() * E;

        //balanced Gram-Schmidt正交化
        vec3 T = TB[0],B = TB[1];
        double factor = .25;              
        for (int i = 0; i < 10; ++i) {                                                             
            T = normalized(T);
            N = normalized(N);
            B = normalized(B);

            vec3 T1 = T - factor * ((T * N) / (N * N)) * N - factor * ((T * B) / (B * B)) * B;     
            vec3 B1 = B - factor * ((B * T) / (T * T)) * T - factor * ((B * N) / (N * N)) * N;     
            vec3 N1 = N - factor * ((N * T) / (T * T)) * T - factor * ((N * B) / (B * B)) * B;     

            T = T1;                                                                                
            N = N1;                                                                                
            B = B1;                                                                                
        }                                     
        T = T - ((T * N) / (N * N)) * N;                                                           
        B = cross(N,T);
        mat<3, 3>TBN{T,B,N};
       
        vec3 n = TBN*model.normal(barUV).xyz();

        double ambient = .4;
        double diffuse = .4 * std::max(0., n * l);
        vec3 r = normalized(2 * n * (n * l) - l);
        double specular = 10. * sampler2D(model.specularmap, barUV)[0] / 255. * std::pow(std::max((vec3{ 0,0,1 }*r), 0.), 35);

        TGAColor gl_Fragment = sampler2D(model.diffusemap, barUV);
        for (int channel : {0, 1, 2}) {
            gl_Fragment[channel] = std::min(255., gl_Fragment[channel] * (ambient + diffuse + specular));
        }
        return { false,gl_Fragment }; 
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
        std::cerr << argv[m]<<std::endl;
        Blinn_PhongShader shader({ -1, 1, 1 }, model);
        std::string fname(argv[m]);
        //if (fname.find("eye_outer") != std::string::npos)
            //shader.ModelTrans = shader.ModelTrans*mat<4,4>{ {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0.1,1}} };//修复bug
        for (int i = 0; i < model.nfaces(); i++) {
            Triangle clip = {shader.vertex(i, 0), shader.vertex(i, 1), shader.vertex(i, 2)};
            rasterize(clip, shader, framebuffer);
        }
    }

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
    
}