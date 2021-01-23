#pragma once

#include "resource.h"

#include <functional>
#include <iostream>
#include <linalg.h>
#include <memory>

#include <stdio.h>


using namespace linalg::aliases;

namespace cg::renderer
{

// VB - Vertex Buffer type.
// RT - Render Target value type (the one of which is render target).
//     Worst naming ever.
template<typename VB, typename RT>
class rasterizer
{
public:
    rasterizer(){};
    ~rasterizer(){};
    void set_render_target(
        std::shared_ptr<resource<RT>> in_render_target,
        std::shared_ptr<resource<float>> in_depth_buffer = nullptr);
    void clear_render_target(const RT& in_clear_value, const float in_depth = FLT_MAX);

    void set_vertex_buffer(std::shared_ptr<resource<VB>> in_vertex_buffer);

    void set_viewport(size_t in_width, size_t in_height);

    void draw(size_t num_vertexes, size_t vertex_offest);

    std::function<std::pair<float4, VB>(float4 vertex, VB vertex_data)> vertex_shader;
    std::function<cg::color(const VB& vertex_data, const float z)> pixel_shader;

protected:
    std::shared_ptr<cg::resource<VB>> vertex_buffer;
    std::shared_ptr<cg::resource<RT>> render_target;
    std::shared_ptr<cg::resource<float>> depth_buffer;

    size_t width = 1920;
    size_t height = 1080;

    float edge_function(float2 a, float2 b, float2 c);
    bool depth_test(float z, size_t x, size_t y);
};

template<typename VB, typename RT>
inline void rasterizer<VB, RT>::set_render_target(
    std::shared_ptr<resource<RT>> in_render_target,
    std::shared_ptr<resource<float>> in_depth_buffer)
{
    if (in_render_target)
        render_target = in_render_target;
}

template<typename VB, typename RT>
inline void rasterizer<VB, RT>::clear_render_target(const RT& in_clear_value, const float in_depth)
{
    if (render_target) {
        for (size_t i = 0; i < render_target->get_number_of_elements(); i++) {
            render_target->item(i) = in_clear_value;
        }
    }
}

template<typename VB, typename RT>
inline void rasterizer<VB, RT>::set_vertex_buffer(std::shared_ptr<resource<VB>> in_vertex_buffer)
{
    vertex_buffer = in_vertex_buffer;
}

template<typename VB, typename RT>
inline void rasterizer<VB, RT>::set_viewport(size_t in_width, size_t in_height)
{
    width = in_width;
    height = in_height;
}

template<typename VB, typename RT>
inline void rasterizer<VB, RT>::draw(size_t num_vertices, size_t vertex_offset)
{
    size_t vertex_id = vertex_offset;
    while (vertex_id < vertex_offset + num_vertices) {
        // Input assembler

        // Read one triangle
        std::vector<VB> vertices(3);
        vertices[0] = vertex_buffer->item(vertex_id++);
        vertices[1] = vertex_buffer->item(vertex_id++);
        vertices[2] = vertex_buffer->item(vertex_id++);

        for (auto& vertex : vertices) {
            float4 coords {vertex.x, vertex.y, vertex.z, 1.f};
            // printf("Coords: %f %f %f\n", vertex.x, vertex.y, vertex.z);

            // Invoke a vertex shader
            auto processed_vertex = vertex_shader(coords, vertex);

            // Convert results from homogeneous back to Cartesian coordinates
            vertex.x = processed_vertex.first.x / processed_vertex.first.w;
            vertex.y = processed_vertex.first.y / processed_vertex.first.w;
            vertex.z = processed_vertex.first.z / processed_vertex.first.w;

            vertex.x = (vertex.x + 1.f) * width / 2.f;
            vertex.y = (-vertex.y + 1.f) * height / 2.f;
        }

        // Compute bounding box to optimize pixel shader invocation to only this region
        float2 bounding_box_begin {
            std::clamp(std::min(std::min(vertices[0].x, vertices[1].x), vertices[2].x), 0.f, static_cast<float>(width) - 1.f),
            std::clamp(std::min(std::min(vertices[0].y, vertices[1].y), vertices[2].y), 0.f, static_cast<float>(height) - 1.f),
        };
        float2 bounding_box_end {
            std::clamp(std::max(std::max(vertices[0].x, vertices[1].x), vertices[2].x), 0.f, static_cast<float>(width) - 1.f),
            std::clamp(std::max(std::max(vertices[0].y, vertices[1].y), vertices[2].y), 0.f, static_cast<float>(height) - 1.f),
        };

        for (int x = static_cast<int>(bounding_box_begin.x); x <= static_cast<int>(bounding_box_end.x); x++)
        for (int y = static_cast<int>(bounding_box_begin.y); y <= static_cast<int>(bounding_box_end.y); y++) {
            float edge0 = edge_function(
                float2 {vertices[0].x, vertices[0].y},
                float2 {vertices[1].x, vertices[1].y},
                float2 {static_cast<float>(x), static_cast<float>(y)}
            );
            float edge1 = edge_function(
                float2 {vertices[1].x, vertices[1].y},
                float2 {vertices[2].x, vertices[2].y},
                float2 {static_cast<float>(x), static_cast<float>(y)}
            );
            float edge2 = edge_function(
                float2 {vertices[2].x, vertices[2].y},
                float2 {vertices[0].x, vertices[0].y},
                float2 {static_cast<float>(x), static_cast<float>(y)}
            );

            if (edge0 >= 0.f && edge1 >= 0.f && edge2 >= 0.f) {
                // We are processing the pixel of a triangle; invoke pixel shader
                // and write the result to render target
                // TODO: fix this
                auto pixel_shader_result = pixel_shader(vertices[0], 0);

                render_target->item(x, y) = RT::from_color(pixel_shader_result);
            }
        }
    }
}

template<typename VB, typename RT>
inline float rasterizer<VB, RT>::edge_function(float2 a, float2 b, float2 c)
{
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

template<typename VB, typename RT>
inline bool rasterizer<VB, RT>::depth_test(float z, size_t x, size_t y)
{
    THROW_ERROR("Not implemented yet");
}

} // namespace cg::renderer
