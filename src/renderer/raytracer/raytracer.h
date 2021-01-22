#pragma once

#include "resource.h"

#include <linalg.h>
#include <memory>
#include <omp.h>
#include <random>
#include <time.h>

using namespace linalg::aliases;

namespace cg::renderer
{
struct ray
{
	ray(float3 position, float3 direction) : position(position)
	{
		THROW_ERROR("Not implemented yet");
	}
	float3 position;
	float3 direction;
};

struct payload
{
	float t;
	float3 bary;
	cg::color color;
};

template<typename VB>
struct triangle
{
	triangle(const VB& vertex_a, const VB& vertex_b, const VB& vertex_c);

	float3 a;
	float3 b;
	float3 c;

	float3 ba;
	float3 ca;

	float3 na;
	float3 nb;
	float3 nc;

	float3 ambient;
	float3 diffuse;
	float3 emissive;
};

template<typename VB>
inline triangle<VB>::triangle(const VB& vertex_a, const VB& vertex_b, const VB& vertex_c)
{
	a = float3{ vertex_a.x, vertex_a.y, vertex_a.z };
	b = float3{ vertex_b.x, vertex_b.y, vertex_b.z };
	c = float3{ vertex_c.x, vertex_c.y, vertex_c.z };

	ba = b - a;
	ca = c - a;

	na = float3{ vertex_a.nx, vertex_a.ny, vertex_a.nz };
	nb = float3{ vertex_b.nx, vertex_b.ny, vertex_b.nz };
	nc = float3{ vertex_c.nx, vertex_c.ny, vertex_c.nz };

	ambient = {
		vertex_a.ambient_r,
		vertex_a.ambient_g,
		vertex_a.ambient_b,
	};
	diffuse = {
		vertex_a.diffuse_r,
		vertex_a.diffuse_g,
		vertex_a.diffuse_b,
	};
	emissive = {
		vertex_a.emissive_r,
		vertex_a.emissive_g,
		vertex_a.emissive_b,
	};
}

template<typename VB>
class aabb
{
public:
	void add_triangle(const triangle<VB> triangle);
	const std::vector<triangle<VB>>& get_traingles() const;
	bool aabb_test(const ray& ray) const;

protected:
	std::vector<triangle<VB>> triangles;

	float3 aabb_min;
	float3 aabb_max;
};

struct light
{
	float3 position;
	float3 color;
};

template<typename VB, typename RT>
class raytracer
{
public:
	raytracer(){};
	~raytracer(){};

	void set_render_target(std::shared_ptr<resource<RT>> in_render_target);
	void clear_render_target(const RT& in_clear_value);
	void set_viewport(size_t in_width, size_t in_height);

	void set_per_shape_vertex_buffer(
		std::vector<std::shared_ptr<cg::resource<VB>>> in_per_shape_vertex_buffer);
	void build_acceleration_structure();
	std::vector<aabb<VB>> acceleration_structures;

	void ray_generation(float3 position, float3 direction, float3 right, float3 up);

	payload trace_ray(const ray& ray, size_t depth, float max_t = 1000.f, float min_t = 0.001f) const;
	payload intersection_shader(const triangle<VB>& triangle, const ray& ray) const;

	std::function<payload(const ray& ray)> miss_shader = nullptr;
	std::function<payload(const ray& ray, payload& payload, const triangle<VB>& triangle)> closest_hit_shader =
		nullptr;
	std::function<payload(const ray& ray, payload& payload, const triangle<VB>& triangle)> any_hit_shader =
		nullptr;


protected:
	std::shared_ptr<cg::resource<RT>> render_target;
	std::vector<std::shared_ptr<cg::resource<VB>>> per_shape_vertex_buffer;

	float get_random(const int thread_num, float range = 0.1f) const;

	size_t width = 1920;
	size_t height = 1080;
};

template<typename VB, typename RT>
inline void raytracer<VB, RT>::set_render_target(std::shared_ptr<resource<RT>> in_render_target)
{
	THROW_ERROR("Not implemented yet");
}

template<typename VB, typename RT>
inline void raytracer<VB, RT>::clear_render_target(const RT& in_clear_value)
{
	THROW_ERROR("Not implemented yet");
}

template<typename VB, typename RT>
inline void raytracer<VB, RT>::set_per_shape_vertex_buffer(
	std::vector<std::shared_ptr<cg::resource<VB>>> in_per_shape_vertex_buffer)
{
	THROW_ERROR("Not implemented yet");
}

template<typename VB, typename RT>
inline void raytracer<VB, RT>::build_acceleration_structure()
{
	THROW_ERROR("Not implemented yet");
}

template<typename VB, typename RT>
inline void raytracer<VB, RT>::set_viewport(size_t in_width, size_t in_height)
{
	THROW_ERROR("Not implemented yet");
}

template<typename VB, typename RT>
inline void raytracer<VB, RT>::ray_generation(
	float3 position, float3 direction, float3 right, float3 up)
{
	THROW_ERROR("Not implemented yet");
}

template<typename VB, typename RT>
inline payload
	raytracer<VB, RT>::trace_ray(const ray& ray, size_t depth, float max_t, float min_t) const
{
	THROW_ERROR("Not implemented yet");
}

template<typename VB, typename RT>
inline payload
	raytracer<VB, RT>::intersection_shader(const triangle<VB>& triangle, const ray& ray) const
{
	THROW_ERROR("Not implemented yet");
	return payload{};
}

template<typename VB, typename RT>
inline float raytracer<VB, RT>::get_random(const int thread_num, const float range) const
{
	static std::default_random_engine generator(thread_num);
	static std::normal_distribution<float> distribution(0.f, range);
	return distribution(generator);
}


template<typename VB>
inline void aabb<VB>::add_triangle(const triangle<VB> triangle)
{
	THROW_ERROR("Not implemented yet");
}

template<typename VB>
inline const std::vector<triangle<VB>>& aabb<VB>::get_traingles() const
{
	return triangles;
}

template<typename VB>
inline bool aabb<VB>::aabb_test(const ray& ray) const
{
	THROW_ERROR("Not implemented yet");
	return false;
}

} // namespace cg::renderer