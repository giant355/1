#pragma once
#include "tgaimage.h"
#include "geometry.h"
#include "model.h"

void lookAt(const vec3& eye, const vec3& center, const vec3& up);
void init_perspective(const double f);
void init_viewport(const int x, const int y, const int w, const int h);
void init_zbuffer(const int width, const int height);

struct Shader {
    virtual void vertex(vec4 v) const= 0;
    virtual std::pair<bool, TGAColor> fragment(const vec3 bar) const = 0;
};

/// <summary>
/// 对模型的顶点进行mvp变化，得到裁剪空间坐标（直接修改了模型的顶点位置）
/// </summary>
/// <param name="shader">提供vertex的shader</param>
/// <param name="model">Model实例</param>
void vertexProcessing(const Shader& shader, Model& model);

typedef vec4 Triangle[3]; // a triangle primitive is made of three ordered points

/// <summary>
/// 把裁剪空间坐标转换到屏幕空间并光栅化三角形
/// </summary>
/// <param name="clip">裁剪空间中的三角形（三个顶点）</param>
/// <param name="shader">着色器，提供 fragment 方法</param>
/// <param name="framebuffer">目标帧缓冲</param>
void rasterize(const Triangle& clip, const Shader& shader, TGAImage& framebuffer);