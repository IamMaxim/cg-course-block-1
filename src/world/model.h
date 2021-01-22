#pragma once

#include "resource.h"

#include <filesystem>
#include <linalg.h>
#include <tiny_obj_loader.h>


using namespace linalg::aliases;

namespace cg::world
{
class model
{
public:
	model();
	virtual ~model();

	void load_obj(const std::filesystem::path& model_path);
	std::shared_ptr<cg::resource<cg::vertex>> get_vertex_buffer() const;
	std::vector<std::shared_ptr<cg::resource<cg::vertex>>> get_per_shape_buffer() const;

	const float4x4 get_world_matrix() const;

protected:
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::shared_ptr<cg::resource<cg::vertex>> vertex_buffer;
	std::vector<std::shared_ptr<cg::resource<cg::vertex>>> per_shape_buffer;
};
} // namespace cg::world