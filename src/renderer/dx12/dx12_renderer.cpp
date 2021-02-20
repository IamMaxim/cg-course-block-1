#include "dx12_renderer.h"

#include "utils/com_error_handler.h"
#include "utils/window.h"

#include <filesystem>


void cg::renderer::dx12_renderer::init()
{
    // Default values
    rtv_descriptor_size = 0;
    view_port = CD3DX12_VIEWPORT(0.f, 0.f, static_cast<float>(settings->width), static_cast<float>(settings->height));
    scissor_rect = CD3DX12_RECT(0, 0, static_cast<LONG>(settings->width), static_cast<LONG>(settings->height));
    vertex_buffer_view = {};

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
    camera->set_position(float3{ settings->camera_position[0], settings->camera_position[1], settings->camera_position[2] });
    camera->set_theta(settings->camera_theta);
    camera->set_phi(settings->camera_phi);
    camera->set_angle_of_view(settings->camera_angle_of_view);
    camera->set_z_near(settings->camera_z_near);
    camera->set_z_far(settings->camera_z_far);


    // TODO: verify matrices
    world_view_projection = camera->get_dxm_view_matrix() * camera->get_dxm_projection_matrix();


    load_pipeline();
    load_assets();
}

void cg::renderer::dx12_renderer::destroy()
{
    wait_for_gpu();
    CloseHandle(fence_event);
}

void cg::renderer::dx12_renderer::update()
{
    world_view_projection = camera->get_dxm_view_matrix() * camera->get_dxm_projection_matrix();
    memcpy(constant_buffer_data_begin, &world_view_projection, sizeof(world_view_projection));
}

void cg::renderer::dx12_renderer::render()
{
    populate_command_list();

    ID3D12CommandList* command_lists[] = { command_list.Get() };
    command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);

    THROW_IF_FAILED(swap_chain->Present(0, 0));

    move_to_next_frame();
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
    THROW_IF_FAILED(D3D12CreateDevice(hardware_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)));

    // Create command queue
    D3D12_COMMAND_QUEUE_DESC queue_descriptor = {};
    queue_descriptor.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queue_descriptor.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    THROW_IF_FAILED(device->CreateCommandQueue(&queue_descriptor, IID_PPV_ARGS(&command_queue)));

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
        command_queue.Get(), cg::utils::window::get_hwnd(), &swap_chain_desc, nullptr, nullptr, &temp_swap_chain));

    dxgi_factory->MakeWindowAssociation(cg::utils::window::get_hwnd(), DXGI_MWA_NO_ALT_ENTER);

    THROW_IF_FAILED(temp_swap_chain.As(&swap_chain));

    frame_index = swap_chain->GetCurrentBackBufferIndex();


    // Create render target view descriptor heap
    D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {};
    rtv_heap_desc.NumDescriptors = frame_number;
    rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    THROW_IF_FAILED(device->CreateDescriptorHeap(&rtv_heap_desc, IID_PPV_ARGS(&rtv_heap)));

    rtv_descriptor_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    // Create render target view for RTs
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(rtv_heap->GetCPUDescriptorHandleForHeapStart());

    for (UINT i = 0; i < frame_number; i++)
    {
        THROW_IF_FAILED(swap_chain->GetBuffer(i, IID_PPV_ARGS(&render_targets[i])));
        device->CreateRenderTargetView(render_targets[i].Get(), nullptr, rtv_handle);

        rtv_handle.Offset(1, rtv_descriptor_size);

        // TODO: add name here
    }

    // Create descriptor for cbv heap
    D3D12_DESCRIPTOR_HEAP_DESC cbv_heap_desc = {};
    cbv_heap_desc.NumDescriptors = 1;
    cbv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    THROW_IF_FAILED(device->CreateDescriptorHeap(&cbv_heap_desc, IID_PPV_ARGS(&cbv_heap)));


    // Create command allocators
    for (size_t i = 0; i < frame_number; i++)
    {
        THROW_IF_FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&command_allocators[i])));
    }
}

void cg::renderer::dx12_renderer::load_assets()
{
    // Create root signature
    D3D12_FEATURE_DATA_ROOT_SIGNATURE rs_feature_data = {};
    rs_feature_data.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

    if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &rs_feature_data, sizeof(rs_feature_data))))
    {
        rs_feature_data.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }

    // Create descriptor tables
    CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
    CD3DX12_ROOT_PARAMETER1 root_parameters[1];
    ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
    root_parameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_VERTEX);

    D3D12_ROOT_SIGNATURE_FLAGS rs_flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
                                          D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
                                          D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
                                          D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
                                          D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rs_descriptor;
    rs_descriptor.Init_1_1(_countof(root_parameters), root_parameters, 0, nullptr, rs_flags);

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;

    auto result = D3DX12SerializeVersionedRootSignature(&rs_descriptor, rs_feature_data.HighestVersion, &signature, &error);

    if (FAILED(result))
    {
        OutputDebugStringA(static_cast<char*>(error->GetBufferPointer()));
        THROW_IF_FAILED(result);
    }

    THROW_IF_FAILED(
        device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&root_signature)));


    // Compile shaders
    WCHAR buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    auto shader_path = std::filesystem::path(buffer).parent_path() / "shaders.hlsl";

    ComPtr<ID3DBlob> vertex_shader;
    ComPtr<ID3DBlob> pixel_shader;

    UINT compile_flags = 0;
#ifdef _DEBUG
    compile_flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif


    // Load vertex shader
    result = D3DCompileFromFile(
        shader_path.wstring().c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compile_flags, 0, &vertex_shader, &error);

    if (FAILED(result))
    {
        OutputDebugStringA(static_cast<char*>(error->GetBufferPointer()));
        THROW_IF_FAILED(result);
    }

    // Load pixel shader
    result = D3DCompileFromFile(
        shader_path.wstring().c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compile_flags, 0, &pixel_shader, &error);

    if (FAILED(result))
    {
        OutputDebugStringA(static_cast<char*>(error->GetBufferPointer()));
        THROW_IF_FAILED(result);
    }


    // Create pipeline state object
    D3D12_INPUT_ELEMENT_DESC input_element_descriptors[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 2, DXGI_FORMAT_R32G32B32_FLOAT, 0, 48, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };


    // Create pipeline state object descriptor
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_descriptor = {};
    pso_descriptor.InputLayout = { input_element_descriptors, _countof(input_element_descriptors) };
    pso_descriptor.pRootSignature = root_signature.Get();
    pso_descriptor.VS = CD3DX12_SHADER_BYTECODE(vertex_shader.Get());
    pso_descriptor.PS = CD3DX12_SHADER_BYTECODE(pixel_shader.Get());
    pso_descriptor.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pso_descriptor.RasterizerState.FrontCounterClockwise = TRUE;
    pso_descriptor.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    pso_descriptor.RasterizerState.DepthClipEnable = FALSE;
    pso_descriptor.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pso_descriptor.DepthStencilState.DepthEnable = FALSE;
    pso_descriptor.DepthStencilState.StencilEnable = FALSE;
    pso_descriptor.SampleMask = UINT_MAX;
    pso_descriptor.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pso_descriptor.NumRenderTargets = 1;
    pso_descriptor.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    pso_descriptor.SampleDesc.Count = 1;

    // Create the pipeline
    THROW_IF_FAILED(device->CreateGraphicsPipelineState(&pso_descriptor, IID_PPV_ARGS(&pipeline_state)));


    // Create command list
    THROW_IF_FAILED(device->CreateCommandList(
        0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocators[0].Get(), pipeline_state.Get(), IID_PPV_ARGS(&command_list)));
    THROW_IF_FAILED(command_list->Close());


    // *** Create and upload vertex buffer *** //
    auto vertex_buffer_data = model->get_vertex_buffer();
    const UINT vertex_buffer_size = static_cast<UINT>(vertex_buffer_data->get_size_in_bytes());

    THROW_IF_FAILED(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(vertex_buffer_size),
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertex_buffer)));

    vertex_buffer->SetName(L"Vertex buffer");

    UINT8* vertex_data_begin;
    CD3DX12_RANGE read_range(0, 0);
    // Map VRAM to RAM to copy data
    THROW_IF_FAILED(vertex_buffer->Map(0, &read_range, reinterpret_cast<void**>(&vertex_data_begin)));
    memcpy(vertex_data_begin, vertex_buffer_data->get_data(), vertex_buffer_size);
    // Unmap VRAM from RAM, since we won't update data anymore
    vertex_buffer->Unmap(0, nullptr);


    // Create vertex buffer descriptor
    vertex_buffer_view.BufferLocation = vertex_buffer->GetGPUVirtualAddress();
    vertex_buffer_view.SizeInBytes = vertex_buffer_size;
    vertex_buffer_view.StrideInBytes = sizeof(cg::vertex);


    // *** Create and upload constant buffer *** ///
    THROW_IF_FAILED(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(64 * 1024), // 64KB
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&constant_buffer)));

    constant_buffer->SetName(L"Constant buffer");

    // Map VRAM to RAM to copy data
    THROW_IF_FAILED(constant_buffer->Map(0, &read_range, reinterpret_cast<void**>(&constant_buffer_data_begin)));
    memcpy(constant_buffer_data_begin, &world_view_projection, sizeof(world_view_projection));
    // Don't unmap VRAM from RAM, since we will update it each frame


    // Create CBV descriptor
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_descriptor = {};
    cbv_descriptor.BufferLocation = constant_buffer->GetGPUVirtualAddress();
    cbv_descriptor.SizeInBytes = (sizeof(world_view_projection) + 255) & ~255;

    device->CreateConstantBufferView(&cbv_descriptor, cbv_heap->GetCPUDescriptorHandleForHeapStart());


    // Create the fence (the thing used to sync CPU and GPU execution)
    THROW_IF_FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
    fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (fence_event == nullptr)
    {
        THROW_IF_FAILED(HRESULT_FROM_WIN32(GetLastError()));
    }

    wait_for_gpu();
}

void cg::renderer::dx12_renderer::populate_command_list()
{
    // Reset the command allocator. If this is failed, something is wrong with
    // our synchronization code
    THROW_IF_FAILED(command_allocators[frame_index]->Reset());
    THROW_IF_FAILED(command_list->Reset(command_allocators[frame_index].Get(), pipeline_state.Get()));

    // Initial state
    command_list->SetGraphicsRootSignature(root_signature.Get());
    ID3D12DescriptorHeap* heaps[] = { cbv_heap.Get() };
    command_list->SetDescriptorHeaps(_countof(heaps), heaps);
    command_list->SetGraphicsRootDescriptorTable(0, cbv_heap->GetGPUDescriptorHandleForHeapStart());
    command_list->RSSetViewports(1, &view_port);
    command_list->RSSetScissorRects(1, &scissor_rect);

    command_list->ResourceBarrier(
        1, &CD3DX12_RESOURCE_BARRIER::Transition(
               render_targets[frame_index].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));


    // Drawing
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(rtv_heap->GetCPUDescriptorHandleForHeapStart(), frame_index, rtv_descriptor_size);
    command_list->OMSetRenderTargets(1, &rtv_handle, FALSE, nullptr);
    const float clear_color[] = { 0.f, 0.f, 0.f, 1.f };
    command_list->ClearRenderTargetView(rtv_handle, clear_color, 0, nullptr);
    command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    command_list->IASetVertexBuffers(0, 1, &vertex_buffer_view);

    command_list->DrawInstanced(model->get_vertex_buffer()->get_number_of_elements(), 1, 0, 0);

    command_list->ResourceBarrier(
        1, &CD3DX12_RESOURCE_BARRIER::Transition(
               render_targets[frame_index].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	THROW_IF_FAILED(command_list->Close());
}


void cg::renderer::dx12_renderer::move_to_next_frame()
{
    const auto current_fence_value = fence_values[frame_index];
    THROW_IF_FAILED(command_queue->Signal(fence.Get(), current_fence_value))

    frame_index = swap_chain->GetCurrentBackBufferIndex();

    if (fence->GetCompletedValue() < fence_values[frame_index])
    {
        THROW_IF_FAILED(fence->SetEventOnCompletion(fence_values[frame_index], fence_event))
    }

    WaitForSingleObjectEx(fence_event, INFINITE, FALSE);

    fence_values[frame_index] = current_fence_value + 1;
}

void cg::renderer::dx12_renderer::wait_for_gpu()
{
    THROW_IF_FAILED(command_queue->Signal(fence.Get(), fence_values[frame_index]))

    THROW_IF_FAILED(fence->SetEventOnCompletion(fence_values[frame_index], fence_event))

    WaitForSingleObjectEx(fence_event, INFINITE, FALSE);

    fence_values[frame_index]++;
}
