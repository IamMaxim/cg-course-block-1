#include "renderer.h"

#include "utils/error_handler.h"

#ifdef RASTERIZATION
#include "renderer/rasterizer/rasterizer_renderer.h"
#endif

#ifdef RAYTRACING
#include "renderer/raytracer/raytracer_renderer.h"
#endif

#ifdef DX12
#include "renderer/dx12/dx12_renderer.h"
#endif


using namespace cg::renderer;

void cg::renderer::renderer::set_settings(std::shared_ptr<cg::settings> in_settings)
{
	settings = in_settings;
}

unsigned cg::renderer::renderer::get_height()
{
	return settings->height;
}

unsigned cg::renderer::renderer::get_width()
{
	return settings->width;
}


std::shared_ptr<renderer> cg::renderer::make_renderer(std::shared_ptr<cg::settings> settings)
{
#ifdef RASTERIZATION
	auto renderer = std::make_shared<cg::renderer::rasterization_renderer>();
	renderer->set_settings(settings);
	return renderer;
#endif
#ifdef RAYTRACING
	auto renderer = std::make_shared<cg::renderer::ray_tracing_renderer>();
	renderer->set_settings(settings);
	return renderer;
#endif
#ifdef DX12
	auto renderer = std::make_shared<cg::renderer::dx12_renderer>();
	renderer->set_settings(settings);
	return renderer;
#endif

	THROW_ERROR("Type of renderer is not selected");
}

void cg::renderer::renderer::move_forward(float delta)
{
	THROW_ERROR("Not implemented yet")
}

void cg::renderer::renderer::move_backward(float delta)
{
	THROW_ERROR("Not implemented yet")
}

void cg::renderer::renderer::move_left(float delta)
{
	THROW_ERROR("Not implemented yet")
}

void cg::renderer::renderer::move_right(float delta)
{
	THROW_ERROR("Not implemented yet")
}

void cg::renderer::renderer::move_yaw(float delta)
{
	THROW_ERROR("Not implemented yet")
}

void cg::renderer::renderer::move_pitch(float delta)
{
	THROW_ERROR("Not implemented yet")
}
