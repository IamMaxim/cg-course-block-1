#define _USE_MATH_DEFINES

#include "camera.h"

#include "utils/error_handler.h"

#include <math.h>


using namespace cg::world;

cg::world::camera::camera() :
theta(0.f), phi(0.f), height(1080.f), width(1920.f), aspect_ratio(1920.f / 1080.f), angle_of_view(1.04719f), z_near(0.001f),
z_far(100.f), position(float3{ 0.f, 0.f, 0.f })
{
}

cg::world::camera::~camera() {}

void cg::world::camera::set_position(float3 in_position)
{
	THROW_ERROR("Not implemented yet");
}

void cg::world::camera::set_theta(float in_theta)
{
	THROW_ERROR("Not implemented yet");
}

void cg::world::camera::set_phi(float in_phi)
{
	THROW_ERROR("Not implemented yet");
}

void cg::world::camera::set_angle_of_view(float in_aov)
{
	THROW_ERROR("Not implemented yet");
}

void cg::world::camera::set_height(float in_height)
{
	THROW_ERROR("Not implemented yet");
}

void cg::world::camera::set_width(float in_width)
{
	THROW_ERROR("Not implemented yet");
}

void cg::world::camera::set_z_near(float in_z_near)
{
	THROW_ERROR("Not implemented yet");
	}

void cg::world::camera::set_z_far(float in_z_far)
{
	THROW_ERROR("Not implemented yet");
}

const float4x4 cg::world::camera::get_view_matrix() const
{
	THROW_ERROR("Not implemented yet");
	return float4x4();
}

#ifdef DX12
const DirectX::XMMATRIX cg::world::camera::get_dxm_view_matrix() const
{
	THROW_ERROR("Not implemented yet");
	return DirectX::XMMatrixIdentity();
}

const DirectX::XMMATRIX cg::world::camera::get_dxm_projection_matrix() const
{
	THROW_ERROR("Not implemented yet");
	return DirectX::XMMatrixIdentity();
}
#endif

const float4x4 cg::world::camera::get_projection_matrix() const
{
	THROW_ERROR("Not implemented yet");
	return float4x4{};
}

const float3 cg::world::camera::get_position() const
{
	THROW_ERROR("Not implemented yet");
	return float3{};
}

const float3 cg::world::camera::get_direction() const
{
	THROW_ERROR("Not implemented yet");
	return float3{};
}

const float3 cg::world::camera::get_right() const
{
	THROW_ERROR("Not implemented yet");
	return float3{};
}

const float3 cg::world::camera::get_up() const
{
	THROW_ERROR("Not implemented yet");
	return float3{};
}
