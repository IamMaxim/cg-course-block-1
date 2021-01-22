#define CATCH_CONFIG_MAIN

#include "world/camera.h"

#include <catch.hpp>


SCENARIO("Camera produces correct space vectors")
{
	GIVEN("Camera object")
	{
		cg::world::camera camera;
		WHEN("Camera located at (0,0,0) with default direction")
		{
			camera.set_position(float3{ 0.f, 0.f, 0.f });
			camera.set_theta(0.f);
			camera.set_phi(0.f);

			THEN("get_position gives correct position")
			{
				float3 position{ 0.f, 0.f, 0.f };
				REQUIRE(abs(camera.get_position() - position) < float3(FLT_EPSILON));
			}

			THEN("get_direction gives a vector along Z")
			{
				float3 direction{ 0.f, 0.f, -1.f };
				REQUIRE(abs(camera.get_direction() - direction) < float3(FLT_EPSILON));
			};

			THEN("get_up gives a vector along Y")
			{
				float3 up{ 0.f, 1.f, 0.f };
				REQUIRE(abs(camera.get_up() - up) < float3(FLT_EPSILON));
			};

			THEN("get_right gives a vector along X")
			{
				float3 right{ 1.f, 0.f, 0.f };
				REQUIRE(abs(camera.get_right() - right) < float3(FLT_EPSILON));
			};
		}

		WHEN("Camera located at (1,0,0) with right turn direction")
		{
			camera.set_position(float3{ 1.f, 0.f, 0.f });
			camera.set_theta(90.f);
			camera.set_phi(0.f);

			THEN("get_position gives correct position")
			{
				float3 position{ 1.f, 0.f, 0.f };
				REQUIRE(abs(camera.get_position() - position) < float3(FLT_EPSILON));
			}

			THEN("get_direction gives a vector along X")
			{
				float3 direction{ 1.f, 0.f, 0.f };
				REQUIRE(abs(camera.get_direction() - direction) < float3(FLT_EPSILON));
			};

			THEN("get_up gives a vector along Y")
			{
				float3 up{ 0.f, 1.f, 0.f };
				REQUIRE(abs(camera.get_up() - up) < float3(FLT_EPSILON));
			};

			THEN("get_right gives a vector along Z")
			{
				float3 right{ -0.f, 0.f, 1.f };
				REQUIRE(abs(camera.get_right() - right) < float3(FLT_EPSILON));
			};
		}
	}
}

SCENARIO("Camera produces correct view matrix")
{
	GIVEN("Camera object")
	{
		cg::world::camera camera;
		WHEN("Camera located at (0,0,0) with default direction")
		{
			camera.set_position(float3{ 0.f, 0.f, 0.f });
			camera.set_theta(0.f);
			camera.set_phi(0.f);


			THEN("get_view_matrix returns correct matrix")
			{
				float4x4 view_matrix{ { 1.f, 0.f, 0.f, -0.f },
									  { 0.f, 1.f, 0.f, -0.f },
									  { 0.f, 0.f, 1.f, -0.f },
									  { 0.f, 0.f, 0.f, 1.f } };
				REQUIRE(abs(camera.get_view_matrix() - view_matrix) <= float4x4(FLT_EPSILON));
			};
		}

		WHEN("Camera located at (0,0,0) with (90, 0) direction")
		{
			camera.set_position(float3{ 0.f, 0.f, 0.f });
			camera.set_theta(90.f);
			camera.set_phi(0.f);

			THEN("get_view_matrix returns correct matrix")
			{
				float4x4 view_matrix{ { -0.f, 0.f, 1.f, -0.f },
									  { 0.f, 1.f, 0.f, -0.f },
									  { -1.f, 0.f, -0.f, -0.f },
									  { 0.f, 0.f, 0.f, 1.f } };
				REQUIRE(abs(camera.get_view_matrix() - view_matrix) <= float4x4(FLT_EPSILON));
			};
		}
	}
}

SCENARIO("Camera produces correct projection matrix")
{
	GIVEN("Camera object")
	{
		cg::world::camera camera;
		WHEN("Aspect ratio = 16/9, AOV = 60, near = 1, far = 10")
		{
			camera.set_width(1920.f);
			camera.set_height(1080.f);
			camera.set_angle_of_view(60.f);
			camera.set_z_near(1.f);
			camera.set_z_far(10.f);

			THEN("get_view_matrix returns correct matrix")
			{
				float4x4 matrix{ { 0.97428f, 0.f, 0.f, 0.f },
								 { 0.f, 1.73205f, 0.f, 0.f },
								 { 0.f, 0.f, -1.11111f, -1.11111f },
								 { 0.f, 0.f, -1.f, 0.f } };
				REQUIRE((abs(camera.get_projection_matrix() - matrix)) <= float4x4(1.e-5f));
			}
		}
	}
}
