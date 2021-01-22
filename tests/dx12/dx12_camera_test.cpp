#define CATCH_CONFIG_MAIN
#define _XM_NO_INTRINSICS_

#include "world/camera.h"

#include <catch.hpp>


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

			THEN("get_dxm_view_matrix returns correct matrix")
			{
				float4x4 matrix = camera.get_view_matrix();
				DirectX::XMMATRIX view_matrix = camera.get_dxm_view_matrix();

				for (int i = 0; i < 4; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						REQUIRE(abs(matrix[i][j] - view_matrix.m[i][j]) <= FLT_EPSILON);
					}
				}
			};
		}

		WHEN("Camera located at (0,0,0) with (90, 0) direction")
		{
			camera.set_position(float3{ 0.f, 0.f, 0.f });
			camera.set_theta(90.f);
			camera.set_phi(0.f);

			THEN("get_dxm_view_matrix returns correct matrix")
			{
				float4x4 matrix = camera.get_view_matrix();
				DirectX::XMMATRIX view_matrix = camera.get_dxm_view_matrix();

				for (int i = 0; i < 4; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						REQUIRE(abs(matrix[i][j] - view_matrix.m[i][j]) <= FLT_EPSILON);
					}
				}
			};
		}

		WHEN("Camera located at (1,1,1) with (45, 15) direction")
		{
			camera.set_position(float3{ 1.f, 1.f, 1.f });
			camera.set_theta(45.f);
			camera.set_phi(15.f);

			THEN("get_dxm_view_matrix returns correct matrix")
			{
				float4x4 matrix = camera.get_view_matrix();
				DirectX::XMMATRIX view_matrix = camera.get_dxm_view_matrix();

				for (int i = 0; i < 4; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						REQUIRE(abs(matrix[i][j] - view_matrix.m[i][j]) <= FLT_EPSILON);
					}
				}
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

			THEN("get_dxm_view_matrix returns correct matrix")
			{
				float4x4 matrix = camera.get_projection_matrix();
				DirectX::XMMATRIX proj_matrix = camera.get_dxm_projection_matrix();

				for (int i = 0; i < 4; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						REQUIRE(abs(matrix[i][j] - proj_matrix.m[i][j]) <= FLT_EPSILON);
					}
				}
			}
		}
	}
}
