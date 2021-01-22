#include "rasterizer_renderer.h"

#include "utils/resource_utils.h"


void cg::renderer::rasterization_renderer::init()
{
    // Create a black-and-white render target with FullHD resolution.
    rasterizer = std::make_shared<cg::renderer::rasterizer<cg::vertex, cg::unsigned_color>>();
	render_target = std::make_shared<cg::resource<cg::unsigned_color>>(
            settings->width,
            settings->height
    );
	rasterizer->set_render_target(render_target);


}

void cg::renderer::rasterization_renderer::destroy() {
}

void cg::renderer::rasterization_renderer::update() {
}

void cg::renderer::rasterization_renderer::render()
{
    rasterizer->clear_render_target({255, 0, 255});

    cg::utils::save_resource(*render_target, settings->result_path);
}
