#define CATCH_CONFIG_MAIN

#include "renderer/rasterizer/rasterizer.h"
#include "resource.h"

#include <catch.hpp>


SCENARIO("Rasterizer produces correct depth buffer")
{
	GIVEN(
		"Vertex buffer with two triangles, render target, depth buffer, and "
		"rasterizer")
	{
		auto vertex_buffer = std::make_shared<cg::resource<cg::vertex>>(6);
		// Only top line
		vertex_buffer->item(0) = { 1.f, 0.8f, 0.5f };
		vertex_buffer->item(1) = { -1.f, 0.8f, 0.5f };
		vertex_buffer->item(2) = { -1.f, 0.5f, 0.5f };
		vertex_buffer->item(3) = { 1.f, 1.f, 1.f };
		vertex_buffer->item(4) = { -1.f, 1.f, 1.f };
		vertex_buffer->item(5) = { -1.f, 0.7f, 1.f };

		auto render_target =
			std::make_shared<cg::resource<cg::unsigned_color>>(10, 10);
		auto depth_buffer = std::make_shared<cg::resource<float>>(10, 10);

		cg::renderer::rasterizer<cg::vertex, cg::unsigned_color> rasterizer;
		rasterizer.set_vertex_buffer(vertex_buffer);
		rasterizer.set_render_target(render_target, depth_buffer);
		rasterizer.set_viewport(10, 10);

		rasterizer.vertex_shader = [](float4 vertex, cg::vertex vertex_data) {
			return std::make_pair(vertex, vertex_data);
		};

		rasterizer.pixel_shader = [](cg::vertex vertex_data, float depth) {
			return cg::color{ depth, depth, depth };
		};

		WHEN("Clear and draw")
		{
			rasterizer.clear_render_target({ 0, 0, 0 });
			rasterizer.draw(vertex_buffer->get_number_of_elements(), 0);

			THEN("Make sure that depth buffer is correct")
			{
				for (size_t x = 0; x < 10; x++)
				{
					REQUIRE(abs(depth_buffer->item(x, 0) - 1.f) < FLT_EPSILON);
					REQUIRE(abs(depth_buffer->item(x, 1) - 0.5f) < FLT_EPSILON);
					REQUIRE(abs(depth_buffer->item(x, 2) - FLT_MAX) < FLT_EPSILON);
				}
			}

			THEN("Make sure that image is correct")
			{
				for (size_t x = 0; x < 10; x++)
				{
					REQUIRE(render_target->item(x, 0).r == 255);
					REQUIRE(render_target->item(x, 0).g == 255);
					REQUIRE(render_target->item(x, 0).b == 255);
					REQUIRE(render_target->item(x, 1).r == 127);
					REQUIRE(render_target->item(x, 1).g == 127);
					REQUIRE(render_target->item(x, 1).b == 127);
				}
			};
		}
	}
}
