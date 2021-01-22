#define CATCH_CONFIG_MAIN

#include "resource.h"
#include "world/model.h"

#include <catch.hpp>


SCENARIO("Loader produces correct vertex buffer resource")
{
	GIVEN("An Obj file with 2 triangles")
	{
		std::filesystem::path obj_file("models/z_test.obj");

		WHEN("Loader load the file")
		{
			cg::world::model model;
			model.load_obj(std::filesystem::absolute(obj_file));

			THEN("The loader is able to return correct vertex buffer")
			{
				cg::resource<cg::vertex> expected_vertex_buffer(6);
				expected_vertex_buffer.item(0) = { -0.5f, 0.5f, -0.5f };
				expected_vertex_buffer.item(1) = { 0.0f, -0.5f, 0.5f };
				expected_vertex_buffer.item(2) = { 0.5f, 0.5f, -0.5f };
				expected_vertex_buffer.item(3) = { -0.5f, -0.5f, -0.5f };
				expected_vertex_buffer.item(4) = { 0.5f, -0.5f, -0.5f };
				expected_vertex_buffer.item(5) = { 0.0f, 0.5f, 0.5f };

				auto vertex_buffer = model.get_vertex_buffer();

				for (size_t i = 0;
					 i < expected_vertex_buffer.get_number_of_elements(); i++)
				{
					REQUIRE(
						abs(expected_vertex_buffer.item(i).x -
							vertex_buffer->item(i).x) <= FLT_EPSILON);
					REQUIRE(
						abs(expected_vertex_buffer.item(i).y -
							vertex_buffer->item(i).y) <= FLT_EPSILON);
					REQUIRE(
						abs(expected_vertex_buffer.item(i).z -
							vertex_buffer->item(i).z) <= FLT_EPSILON);
				}
			};
		}
	}
}
