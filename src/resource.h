#pragma once

#include "utils/error_handler.h"

#include <algorithm>
#include <linalg.h>
#include <vector>


using namespace linalg::aliases;

namespace cg
{
template<typename T>
class resource
{
public:
	resource(size_t size);
	resource(size_t x_size, size_t y_size);
	~resource();

	const T* get_data();
	T& item(size_t item);
	T& item(size_t x, size_t y);

	size_t get_size_in_bytes() const;
	size_t get_number_of_elements() const;
	size_t get_stride() const;

private:
	std::vector<T> data;
	size_t item_size;
	size_t stride;
};
template<typename T>
inline resource<T>::resource(size_t size)
{
	THROW_ERROR("Not implemented yet");
}
template<typename T>
inline resource<T>::resource(size_t x_size, size_t y_size)
{
	THROW_ERROR("Not implemented yet");
}
template<typename T>
inline resource<T>::~resource()
{
}
template<typename T>
inline const T* resource<T>::get_data()
{
	THROW_ERROR("Not implemented yet");
	return nullptr;
}
template<typename T>
inline T& resource<T>::item(size_t item)
{
	THROW_ERROR("Not implemented yet");
	T obj;
	return obj;
}
template<typename T>
inline T& resource<T>::item(size_t x, size_t y)
{
	THROW_ERROR("Not implemented yet");
	T obj;
	return obj;
}
template<typename T>
inline size_t resource<T>::get_size_in_bytes() const
{
	THROW_ERROR("Not implemented yet");
	return 0;
}
template<typename T>
inline size_t resource<T>::get_number_of_elements() const
{
	THROW_ERROR("Not implemented yet");
	return 0;
}

template<typename T>
inline size_t resource<T>::get_stride() const
{
	THROW_ERROR("Not implemented yet");
	return 0;
}
struct color
{
	static color from_float3(const float3& in)
	{
		THROW_ERROR("Not implemented yet");
		return color();
	};
	float r;
	float g;
	float b;
};

struct unsigned_color
{
	static unsigned_color from_color(const color& color)
	{
		THROW_ERROR("Not implemented yet");
		return unsigned_color();
	};
	float3 to_float3()
	{
		THROW_ERROR("Not implemented yet");
		return float3();
	};
	unsigned char r;
	unsigned char g;
	unsigned char b;
};


struct vertex
{
	float x;
	float y;
	float z;

	float nx;
	float ny;
	float nz;

	float ambient_r;
	float ambient_g;
	float ambient_b;

	float diffuse_r;
	float diffuse_g;
	float diffuse_b;

	float emissive_r;
	float emissive_g;
	float emissive_b;
};

} // namespace cg