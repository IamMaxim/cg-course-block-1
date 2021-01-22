#define CATCH_CONFIG_MAIN

#include "renderer/raytracer/raytracer.h"
#include "resource.h"

#include <catch.hpp>


SCENARIO("Raytracer generates rays")
{
	GIVEN("Raytracer with render target")
	{
		cg::renderer::raytracer<cg::vertex, cg::unsigned_color> raytracer;
		auto render_target = std::make_shared<cg::resource<cg::unsigned_color>>(2, 2);
		raytracer.set_render_target(render_target);
		raytracer.set_viewport(2, 2);

		WHEN("Setup miss shader and generate rays")
		{
			raytracer.clear_render_target({ 0, 0, 0 });
			raytracer.miss_shader = [](const cg::renderer::ray& ray) {
				cg::renderer::payload payload = {};
				payload.t = -1.f;
				payload.color = { ray.direction.x / 0.5f + 0.5f,
								  ray.direction.y / 0.5f + 0.5f,
								  ray.direction.z / 0.5f + 0.5f };
				return payload;
			};

			raytracer.ray_generation(
				float3{ 0.f, 0.f, 0.f }, float3{ 0.f, 0.f, 1.f },
				float3{ 1.f, 0.f, 0.f }, float3{ 0.f, 1.f, 0.f });

			THEN("Make sure that image is correct")
			{
				REQUIRE(render_target->item(0, 0).r == 0);
				REQUIRE(render_target->item(0, 0).g == 255);
				REQUIRE(render_target->item(0, 0).b == 255);

				REQUIRE(render_target->item(0, 1).r == 0);
				REQUIRE(render_target->item(0, 1).g == 0);
				REQUIRE(render_target->item(0, 1).b == 255);

				REQUIRE(render_target->item(1, 0).r == 255);
				REQUIRE(render_target->item(1, 0).g == 255);
				REQUIRE(render_target->item(1, 0).b == 255);

				REQUIRE(render_target->item(1, 1).r == 255);
				REQUIRE(render_target->item(1, 1).g == 0);
				REQUIRE(render_target->item(1, 1).b == 255);
			};
		}
	}
}
