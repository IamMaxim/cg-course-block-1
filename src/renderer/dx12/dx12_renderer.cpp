#include "dx12_renderer.h"

#include "utils/com_error_handler.h"
#include "utils/window.h"

#include <filesystem>


void cg::renderer::dx12_renderer::init()
{
	// Default values
	rtv_descriptor_size = 0;
	view_port = CD3DX12_VIEWPORT(
		0.f, 0.f, static_cast<float>(settings->width),
		static_cast<float>(settings->height));
	scissor_rect = CD3DX12_RECT(
		0, 0, static_cast<LONG>(settings->width), static_cast<LONG>(settings->height));
	vertex_buffer_view = {};

	// TODO: verify matrices
	// world_view_projection =
	//   camera->get_dxm_view_matrix() * camera->get_dxm_projection_matrix();
	constant_buffer_data_begin = nullptr;
	frame_index = 0;

	for (size_t i = 0; i < frame_number; i++)
	{
		fence_values[i] = 0;
	}

	// Load model
	model = std::make_shared<cg::world::model>();
	model->load_obj(settings->model_path);

	// Setup camera with command line arguments
	camera = std::make_shared<cg::world::camera>();
	camera->set_width(static_cast<float>(settings->width));
	camera->set_height(static_cast<float>(settings->height));
	camera->set_position(float3{ settings->camera_position[0],
								 settings->camera_position[1],
								 settings->camera_position[2] });
	camera->set_theta(settings->camera_theta);
	camera->set_phi(settings->camera_phi);
	camera->set_angle_of_view(settings->camera_angle_of_view);
	camera->set_z_near(settings->camera_z_near);
	camera->set_z_far(settings->camera_z_far);

	load_pipeline();
	load_assets();
}

void cg::renderer::dx12_renderer::destroy()
{
	THROW_ERROR("Not implemented yet")
}

void cg::renderer::dx12_renderer::update()
{
	THROW_ERROR("Not implemented yet")
}

void cg::renderer::dx12_renderer::render()
{
	THROW_ERROR("Not implemented yet")
}

void cg::renderer::dx12_renderer::load_pipeline()
{
	// Debug layer
	UINT dxgi_factory_flags = 0;

#ifdef _DEBUG
	ComPtr<ID3D12Debug> debug_controller;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller))))
	{
		debug_controller->EnableDebugLayer();
		dxgi_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	// Hardware adapter
	ComPtr<IDXGIFactory4> dxgi_factory;
	THROW_IF_FAILED(CreateDXGIFactory2(dxgi_factory_flags, IID_PPV_ARGS(&dxgi_factory)));

	ComPtr<IDXGIAdapter1> hardware_adapter;
	dxgi_factory->EnumAdapters1(0, &hardware_adapter);

#ifdef _DEBUG
	DXGI_ADAPTER_DESC adapter_desc = {};
	hardware_adapter->GetDesc(&adapter_desc);
	OutputDebugString(adapter_desc.Description);
#endif

	// Create device
	THROW_IF_FAILED(D3D12CreateDevice(
		hardware_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)));

	// Create command queue
	D3D12_COMMAND_QUEUE_DESC queue_descriptor = {};
	queue_descriptor.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queue_descriptor.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	THROW_IF_FAILED(
		device->CreateCommandQueue(&queue_descriptor, IID_PPV_ARGS(&command_queue)));

	// Create swap chain
	DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = {};
	swap_chain_desc.BufferCount = frame_number;
	swap_chain_desc.Width = settings->width;
	swap_chain_desc.Height = settings->height;
	swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swap_chain_desc.SampleDesc.Count = 1;

	ComPtr<IDXGISwapChain1> temp_swap_chain;
	THROW_IF_FAILED(dxgi_factory->CreateSwapChainForHwnd(
		command_queue.Get(), cg::utils::window::get_hwnd(), &swap_chain_desc,
		nullptr, nullptr, &temp_swap_chain));

	dxgi_factory->MakeWindowAssociation(cg::utils::window::get_hwnd(), DXGI_MWA_NO_ALT_ENTER);

	THROW_IF_FAILED(temp_swap_chain.As(&swap_chain));

	frame_index = swap_chain->GetCurrentBackBufferIndex();



}

void cg::renderer::dx12_renderer::load_assets()
{
	THROW_ERROR("Not implemented yet")
}

void cg::renderer::dx12_renderer::populate_command_list()
{
	THROW_ERROR("Not implemented yet")
}


void cg::renderer::dx12_renderer::move_to_next_frame()
{
	THROW_ERROR("Not implemented yet")
}

void cg::renderer::dx12_renderer::wait_for_gpu()
{
	THROW_ERROR("Not implemented yet")
}
