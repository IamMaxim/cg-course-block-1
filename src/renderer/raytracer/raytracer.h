#pragma once

#include "resource.h"

#include <linalg.h>
#include <memory>
#include <omp.h>
#include <random>
#include <stdio.h>
#include <time.h>

using namespace linalg::aliases;

namespace cg::renderer
{
struct ray
{
	ray(float3 position, float3 direction) : position(position)
	{
		this->direction = normalize(direction);
	}
	float3 position;
	float3 direction;
};

struct payload
{
	float t;
	float3 bary;
	cg::color color;
	size_t depth;
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
	// std::vector<triangle<VB>> acceleration_structures;

	void ray_generation(float3 position, float3 direction, float3 right, float3 up);

	payload trace_ray(const ray& ray, size_t depth, float max_t = 1000.f, float min_t = 0.001f) const;
	payload intersection_shader(const triangle<VB>& triangle, const ray& ray) const;

	std::function<payload(const ray& ray)> miss_shader = nullptr;
	std::function<payload(const ray& ray, payload& payload, const triangle<VB>& triangle)> closest_hit_shader =
		nullptr;
	std::function<payload(const ray& ray, payload& payload, const triangle<VB>& triangle)> any_hit_shader =
		nullptr;

	float get_random(const int thread_num, float range = 0.1f) const;

	// You can alter this number to enable/disable super-sampling anti-aliasing.
	// Value of 1 means SSAA is disabled.
	int SSAA_factor = 16;

	// Max amount of times the same ray may be re-casted.
	int max_depth = 5;

protected:
	std::shared_ptr<cg::resource<RT>> render_target;
	std::vector<std::shared_ptr<cg::resource<VB>>> per_shape_vertex_buffer;

	size_t width = 1920;
	size_t height = 1080;
};

template<typename VB, typename RT>
inline void raytracer<VB, RT>::set_render_target(std::shared_ptr<resource<RT>> in_render_target)
{
	render_target = in_render_target;
}

template<typename VB, typename RT>
inline void raytracer<VB, RT>::clear_render_target(const RT& in_clear_value)
{
	for (size_t i = 0; i < render_target->get_number_of_elements(); i++)
	{
		render_target->item(i) = in_clear_value;
	}
}

template<typename VB, typename RT>
inline void raytracer<VB, RT>::set_per_shape_vertex_buffer(
	std::vector<std::shared_ptr<cg::resource<VB>>> in_per_shape_vertex_buffer)
{
	per_shape_vertex_buffer = in_per_shape_vertex_buffer;
}

template<typename VB, typename RT>
inline void raytracer<VB, RT>::build_acceleration_structure()
{
	for (auto& vertex_buffer : per_shape_vertex_buffer)
	{
		size_t vertex_id = 0;
		aabb<VB> aabb;

		while (vertex_id < vertex_buffer->get_number_of_elements())
		{
			triangle<VB> triangle(
				vertex_buffer->item(vertex_id++), vertex_buffer->item(vertex_id++),
				vertex_buffer->item(vertex_id++));

			aabb.add_triangle(triangle);
		}

		acceleration_structures.push_back(aabb);
	}
}

template<typename VB, typename RT>
inline void raytracer<VB, RT>::set_viewport(size_t in_width, size_t in_height)
{
	width = in_width;
	height = in_height;
}

template<typename VB, typename RT>
inline void raytracer<VB, RT>::ray_generation(
	float3 position, float3 direction, float3 right, float3 up)
{
	for (int x = 0; x < width; x++)
	{
#pragma omp parallel for
		for (int y = 0; y < height; y++)
		{
			float3 res_color(0.f);

			for (int px = 0; px < SSAA_factor; px++)
				for (int py = 0; py < SSAA_factor; py++)
				{
					// float x_jitter = get_random(omp_get_thread_num() + clock());
					// float y_jitter = get_random(omp_get_thread_num() + clock());

					// Converting from [0; width - 1] to [-1, 1]:
					//     [0; width - 1] -> [0; 1] -> [0; 2] -> [-1; 1]
					float u = 2.f * (x + px / static_cast<float>(SSAA_factor)) /
								  static_cast<float>(width - 1) -
							  1.f;
					u *= static_cast<float>(width) / static_cast<float>(height);
					float v = 2.f * (y + py / static_cast<float>(SSAA_factor)) /
								  static_cast<float>(height - 1) -
							  1.f;

					float3 ray_direction = direction + (u)*right - (v)*up;
					ray ray(position, ray_direction);

					payload payload = trace_ray(ray, max_depth);

					res_color +=
						float3(payload.color.r, payload.color.g, payload.color.b);

					// cg::color accumed =
					// cg::color::from_float3(render_target->item(x,
					// y).to_float3()); cg::color result{ 	(accumed.r * 4.f +
					// payload.color.r) / 5.f, 	(accumed.g * 4.f +
					// payload.color.g) / 5.f, 	(accumed.b * 4.f +
					// payload.color.b) / 5.f,
					// };
				}

			render_target->item(x, y) = RT::from_color(
				cg::color::from_float3(res_color / SSAA_factor / SSAA_factor));
		}

		printf("\rProgress: %.2f%%", 100.f * x / width);
	}
}

template<typename VB, typename RT>
inline payload
	raytracer<VB, RT>::trace_ray(const ray& ray, size_t depth, float max_t, float min_t) const
{
	if (depth == 0)
		return miss_shader(ray);

	depth--;

	payload closest_hit_payload = {};
	closest_hit_payload.t = max_t;
	const triangle<VB>* closest_triangle = nullptr;

	for (auto& aabb : acceleration_structures)
	{
		if (!aabb.aabb_test(ray))
			continue;

		for (auto& triangle : aabb.get_traingles())
		{
			payload payload = intersection_shader(triangle, ray);

			if (payload.t > min_t && payload.t < closest_hit_payload.t)
			{
				closest_hit_payload = payload;
				closest_triangle = &triangle;

				if (any_hit_shader)
				{
					return any_hit_shader(ray, payload, triangle);
				}
			}
		}
	}

	if (closest_hit_payload.t < max_t)
	{
		if (closest_hit_shader)
		{
			closest_hit_payload.depth = depth;
			return closest_hit_shader(ray, closest_hit_payload, *closest_triangle);
		}
	}

	return miss_shader(ray);
}

template<typename VB, typename RT>
inline payload
	raytracer<VB, RT>::intersection_shader(const triangle<VB>& triangle, const ray& ray) const
{
	payload payload{};
	payload.t = -1.f;

	float3 pvec = cross(ray.direction, triangle.ca);
	float det = dot(triangle.ba, pvec);

	// No intersection; return empty payload
	if (det > -1e-8 && det < 1e-8)
		return payload;

	float inv_det = 1.f / det;

	float3 tvec = ray.position - triangle.a;
	float u = dot(tvec, pvec) * inv_det;
	if (u < 0.f || u > 1.f)
		return payload;

	float3 qvec = cross(tvec, triangle.ba);
	float v = dot(ray.direction, qvec) * inv_det;
	if (v < 0.f || (u + v) > 1.f)
		return payload;

	payload.t = dot(triangle.ca, qvec) * inv_det;
	payload.bary = float3{ 1.f - u - v, u, v };

	return payload;
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
	if (triangles.empty())
		aabb_max = aabb_min = triangle.a;

	triangles.push_back(triangle);

	aabb_max = max(triangle.a, aabb_max);
	aabb_max = max(triangle.b, aabb_max);
	aabb_max = max(triangle.c, aabb_max);

	aabb_min = min(triangle.a, aabb_min);
	aabb_min = min(triangle.b, aabb_min);
	aabb_min = min(triangle.c, aabb_min);
}

template<typename VB>
inline const std::vector<triangle<VB>>& aabb<VB>::get_traingles() const
{
	return triangles;
}

template<typename VB>
inline bool aabb<VB>::aabb_test(const ray& ray) const
{
	float3 inv_ray_direction = float3(1.f) / ray.direction;
	float3 t0 = (aabb_max - ray.position) * inv_ray_direction;
	float3 t1 = (aabb_min - ray.position) * inv_ray_direction;
	float3 tmin = min(t0, t1);
	float3 tmax = max(t0, t1);
	return maxelem(tmin) <= minelem(tmax);
}

} // namespace cg::renderer