#define CATCH_CONFIG_MAIN

#include "renderer/raytracer/raytracer.h"
#include "resource.h"

#include <catch.hpp>


SCENARIO("Raytracer has correct ray-triangle test")
{
	GIVEN("Raytracer, vertex buffer, and render target")
	{
		cg::renderer::raytracer<cg::vertex, cg::unsigned_color> raytracer;
		raytracer.set_viewport(5, 5);


		auto render_target = std::make_shared<cg::resource<cg::unsigned_color>>(5, 5);
		raytracer.set_render_target(render_target);

		std::vector<std::shared_ptr<cg::resource<cg::vertex>>> vertex_buffer;
		vertex_buffer.push_back(std::make_shared<cg::resource<cg::vertex>>(3));
		vertex_buffer[0]->item(0).x = -0.5f;
		vertex_buffer[0]->item(0).y = -0.5f;
		vertex_buffer[0]->item(0).z = -1.f;
		vertex_buffer[0]->item(1).x = 0.5f;
		vertex_buffer[0]->item(1).y = -0.5f;
		vertex_buffer[0]->item(1).z = -1.f;
		vertex_buffer[0]->item(2).x = 0.f;
		vertex_buffer[0]->item(2).y = 0.5f;
		vertex_buffer[0]->item(2).z = -1.f;

		raytracer.set_per_shape_vertex_buffer(vertex_buffer);
		raytracer.build_acceleration_structure();

		WHEN("Setup miss, hit shader and generate rays")
		{
			raytracer.clear_render_target({ 0, 0, 0 });
			raytracer.miss_shader = [](const cg::renderer::ray& ray) {
				cg::renderer::payload payload = {};
				payload.t = -1.f;
				payload.color = { 0.f, 0.f, 0.f };
				return payload;
			};

			raytracer.closest_hit_shader = [](const cg::renderer::ray& ray,
											  cg::renderer::payload& payload,
											  const cg::renderer::triangle<cg::vertex>& triangle) {
				payload.color = cg::color::from_float3(float3{ 1.f, 1.f, 1.f });
				return payload;
			};


			raytracer.ray_generation(
				float3{ 0.f, 0.f, 1.f }, float3{ 0.f, 0.f, -1.f },
				float3{ 1.f, 0.f, 0.f }, float3{ 0.f, 1.f, 0.f });

			THEN("Make sure that image is correct")
			{
				for (size_t x = 0; x < 5; x++)
				{
					for (size_t y = 0; y < 5; y++)
					{
						if (x == 2 && y == 2)
						{
							REQUIRE(render_target->item(x, y).r == 255);
							REQUIRE(render_target->item(x, y).g == 255);
							REQUIRE(render_target->item(x, y).b == 255);
						}
						else
						{
							REQUIRE(render_target->item(x, y).r == 0);
							REQUIRE(render_target->item(x, y).g == 0);
							REQUIRE(render_target->item(x, y).b == 0);
						}
					}
				}
			};
		}
	}
}