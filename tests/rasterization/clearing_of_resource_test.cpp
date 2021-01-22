#define CATCH_CONFIG_MAIN
//#define CATCH_CONFIG_ENABLE_BENCHMARKING

#include "renderer/rasterizer/rasterizer.h"
#include "resource.h"

#include <catch.hpp>


SCENARIO("1D resource could be created and accessed by items", "[resource]")
{
	GIVEN("1D resources (1 byte per item)")
	{
		cg::resource<unsigned char> res(100);
		REQUIRE(res.get_size_in_bytes() == 100);

		WHEN("Items are changed")
		{
			res.item(0) = 128;
			res.item(1) = 255;

			THEN("We can confirm that changes are stored")
			{
				REQUIRE(res.item(0) == 128);
				REQUIRE(res.item(1) == 255);
				REQUIRE(res.item(2) == 0);
			}
		}
	}
}

SCENARIO("2D resource could be created and accessed by items", "[resource]")
{
	GIVEN("2D resource (1 byte per item)")
	{
		cg::resource<unsigned char> res(100, 100);
		REQUIRE(res.get_size_in_bytes() == 10000);

		WHEN("Items are changed")
		{
			res.item(0, 0) = 128;
			res.item(1, 0) = 255;
			res.item(0, 1) = 64;

			THEN("We can confirm that changes are stored")
			{
				REQUIRE(res.item(0, 0) == 128);
				REQUIRE(res.item(1, 0) == 255);
				REQUIRE(res.item(0, 1) == 64);
			}

			THEN("We can access the data using linear nad matrix notations")
			{
				REQUIRE(res.item(0) == res.item(0, 0));
				REQUIRE(res.item(1) == res.item(1, 0));
				REQUIRE(res.item(100) == res.item(0, 1));
			}
		}
	}
}

SCENARIO("Rasterizer can clear render target", "[rasterizer]")
{
	GIVEN("Dummy rasterizer with unsigned char render target")
	{
		cg::renderer::rasterizer<float, unsigned char> rasterizer;
		auto render_target = std::make_shared<cg::resource<unsigned char>>(1920, 1080);
		rasterizer.set_render_target(render_target);

		WHEN("We call clear render target")
		{
			unsigned char clear_color = 128;
			rasterizer.clear_render_target(clear_color);

			THEN("The render target was cleared")
			{
				for (size_t i = 0; i < render_target->get_number_of_elements(); i++)
				{
					REQUIRE(render_target->item(i) == clear_color);
				}
			}
		}
	}
}

/*
TEST_CASE("FullHD clear benchmark", "[benchmark]")
{
	cg::renderer::rasterizer<float, unsigned char> rasterizer;
	auto render_target = std::make_shared<cg::resource<unsigned char>>(1920,
1080); rasterizer.set_render_target(render_target);

	BENCHMARK("FullHD clear benchmark")
	{
		return rasterizer.clear_render_target(0);
	};
}
*/
