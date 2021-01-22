#define CATCH_CONFIG_MAIN

#include "renderer/rasterizer/rasterizer.h"
#include "resource.h"

#include <catch.hpp>


SCENARIO("Rasterizer produces correct image")
{
	GIVEN("Vertex buffer with one triangle, render target, and rasterizer")
	{
		auto vertex_buffer = std::make_shared<cg::resource<cg::vertex>>(3);
		// Only top line
		vertex_buffer->item(0) = { 1.f, 1.f, 0.f };
		vertex_buffer->item(1) = { -1.f, 1.f, 0.f };
		vertex_buffer->item(2) = { -1.f, 0.7f, 0.f };

		auto render_target =
			std::make_shared<cg::resource<cg::unsigned_color>>(10, 10);

		cg::renderer::rasterizer<cg::vertex, cg::unsigned_color> rasterizer;
		rasterizer.set_vertex_buffer(vertex_buffer);
		rasterizer.set_render_target(render_target);
		rasterizer.set_viewport(10, 10);

		rasterizer.vertex_shader = [](float4 vertex, cg::vertex vertex_data) {
			return std::make_pair(vertex, vertex_data);
		};

		rasterizer.pixel_shader = [](cg::vertex vertex_data, float depth) {
			return cg::color{ 1.f, 1.f, 1.f };
		};

		WHEN("Clear and draw")
		{
			rasterizer.clear_render_target({ 0, 0, 0 });
			rasterizer.draw(3, 0);

			THEN("Make sure than image is correct")
			{
				for (size_t x = 0; x < 10; x++)
				{
					REQUIRE(render_target->item(x, 0).r == 255);
					REQUIRE(render_target->item(x, 0).g == 255);
					REQUIRE(render_target->item(x, 0).b == 255);
				}
			};
		}
	}
}
