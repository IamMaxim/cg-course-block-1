#pragma once

#include "resource.h"

#include <functional>
#include <iostream>
#include <linalg.h>
#include <memory>


using namespace linalg::aliases;

namespace cg::renderer
{
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
	THROW_ERROR("Not implemented yet");
}

template<typename VB, typename RT>
inline void rasterizer<VB, RT>::clear_render_target(const RT& in_clear_value, const float in_depth)
{
	THROW_ERROR("Not implemented yet");
}

template<typename VB, typename RT>
inline void rasterizer<VB, RT>::set_vertex_buffer(std::shared_ptr<resource<VB>> in_vertex_buffer)
{
	THROW_ERROR("Not implemented yet");
}

template<typename VB, typename RT>
inline void rasterizer<VB, RT>::set_viewport(size_t in_width, size_t in_height)
{
	THROW_ERROR("Not implemented yet");
}

template<typename VB, typename RT>
inline void rasterizer<VB, RT>::draw(size_t num_vertexes, size_t vertex_offest)
{
	THROW_ERROR("Not implemented yet");
}

template<typename VB, typename RT>
inline float rasterizer<VB, RT>::edge_function(float2 a, float2 b, float2 c)
{
	THROW_ERROR("Not implemented yet");
}

template<typename VB, typename RT>
inline bool rasterizer<VB, RT>::depth_test(float z, size_t x, size_t y)
{
	THROW_ERROR("Not implemented yet");
}

} // namespace cg::renderer