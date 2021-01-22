workspace "Computer graphics in Game development"
    configurations { "Debug", "Release" }
    language "C++"
    cppdialect "C++17"
    architecture "x64"
    systemversion "latest"
    toolset "msc"
    optimize "Speed"
    buildoptions { "/openmp" }
    targetdir ("bin/%{prj.name}/%{cfg.longname}")
    objdir ("obj/%{prj.name}/%{cfg.longname}")
    filter("configurations:Debug")
        defines({ "DEBUG" })
        symbols("On")
        optimize("Off")
    filter("configurations:Release")
        defines({ "NDEBUG" })
        symbols("On")

project "01. Rasterization"
    kind "ConsoleApp"
    defines { "RASTERIZATION" }
    includedirs { "libs/stb", "libs/tinyobjloader", "libs/linalg", "libs/cxxopts/include" }
    includedirs { "src" }
    files { "src/settings.*"}
    files { "src/resource.*" }
    files { "src/renderer/renderer.*"}
    files { "src/renderer/rasterizer/rasterizer.*" }
    files { "src/renderer/rasterizer/rasterizer_renderer.*"}
    files { "src/world/camera.*"}
    files { "src/world/model.*"}
    files { "src/utils/resource_utils.*"}
    files { "src/main.cpp" }

group "Tests"
    -- the project is required for packing of tested code
    project "Static"
        kind "StaticLib"
        defines { "DX12" }
        includedirs { "libs/stb", "libs/tinyobjloader", "libs/linalg", "libs/cxxopts/include", "libs/D3DX12" }
        includedirs { "src" }
        files { "src/settings.*"}
        files { "src/resource.*" }
        files { "src/renderer/renderer.*"}
        files { "src/renderer/rasterizer/rasterizer.*" }
        files { "src/renderer/rasterizer/rasterizer_renderer.*"}
        files { "src/renderer/raytracer/raytracer.*" }
        files { "src/renderer/raytracer/raytracer_renderer.*"}
        files { "src/world/camera.*"}
        files { "src/world/model.*"}
        files { "src/utils/resource_utils.*"}

    project "Test 01. Clearing of resource"
        kind "ConsoleApp"
        defines { "RASTERIZATION" }
        includedirs { "libs/Catch2/single_include/catch2" }
        includedirs { "libs/stb", "libs/tinyobjloader", "libs/linalg", "libs/cxxopts/include" }
        includedirs { "src" }
        links { "Static" }
        files { "tests/rasterization/clearing_of_resource_test.cpp" }

    project "Test 02. Model loader"
        kind "ConsoleApp"
        defines { "RASTERIZATION" }
        includedirs { "libs/Catch2/single_include/catch2" }
        includedirs { "libs/stb", "libs/tinyobjloader", "libs/linalg", "libs/cxxopts/include" }
        includedirs { "src" }
        links { "Static" }
        files { "tests/rasterization/model_loader_test.cpp" }

    project "Test 03. Camera"
        kind "ConsoleApp"
        defines { "RASTERIZATION" }
        includedirs { "libs/Catch2/single_include/catch2" }
        includedirs { "libs/stb", "libs/tinyobjloader", "libs/linalg", "libs/cxxopts/include" }
        includedirs { "src" }
        links { "Static" }
        files { "tests/rasterization/camera_test.cpp" }

    project "Test 04. Rasterization"
        kind "ConsoleApp"
        defines { "RASTERIZATION" }
        includedirs { "libs/Catch2/single_include/catch2" }
        includedirs { "libs/stb", "libs/tinyobjloader", "libs/linalg", "libs/cxxopts/include" }
        includedirs { "src" }
        links { "Static" }
        files { "tests/rasterization/rasterization_test.cpp" }

    project "Test 05. Depth buffer"
        kind "ConsoleApp"
        defines { "RASTERIZATION" }
        includedirs { "libs/Catch2/single_include/catch2" }
        includedirs { "libs/stb", "libs/tinyobjloader", "libs/linalg", "libs/cxxopts/include" }
        includedirs { "src" }
        links { "Static" }
        files { "tests/rasterization/depth_buffer_test.cpp" }

group ""

project "02. Ray tracing"
    kind "ConsoleApp"
    defines { "RAYTRACING" }
    includedirs { "libs/stb", "libs/tinyobjloader", "libs/linalg", "libs/cxxopts/include" }
    includedirs { "src" }
    files { "src/settings.*"}
    files { "src/resource.*" }
    files { "src/renderer/renderer.*"}
    files { "src/renderer/raytracer/raytracer.*" }
    files { "src/renderer/raytracer/raytracer_renderer.*"}
    files { "src/world/camera.*"}
    files { "src/world/model.*"}
    files { "src/utils/resource_utils.*"}
    files { "src/main.cpp" }

group "Tests"
    project "Test 06. Camera ray generation"
        kind "ConsoleApp"
        defines { "RAYTRACING" }
        includedirs { "libs/Catch2/single_include/catch2" }
        includedirs { "libs/stb", "libs/tinyobjloader", "libs/linalg", "libs/cxxopts/include" }
        includedirs { "src" }
        links { "Static" }
        files { "tests/ray_tracing/camera_ray_generation_test.cpp" }

    project "Test 07. Ray-triangle intersection"
        kind "ConsoleApp"
        defines { "RAYTRACING" }
        includedirs { "libs/Catch2/single_include/catch2" }
        includedirs { "libs/stb", "libs/tinyobjloader", "libs/linalg", "libs/cxxopts/include" }
        includedirs { "src" }
        links { "Static" }
        files { "tests/ray_tracing/ray_triangle_intersection_test.cpp" }

    project "Test 08. Lambertian shading"
        kind "ConsoleApp"
        defines { "RAYTRACING" }
        includedirs { "libs/Catch2/single_include/catch2" }
        includedirs { "libs/stb", "libs/tinyobjloader", "libs/linalg", "libs/cxxopts/include" }
        includedirs { "src" }
        links { "Static" }
        files { "tests/ray_tracing/lambertian_shading_test.cpp" }

    project "Test 09. Shadow rays"
        kind "ConsoleApp"
        defines { "RAYTRACING" }
        includedirs { "libs/Catch2/single_include/catch2" }
        includedirs { "libs/stb", "libs/tinyobjloader", "libs/linalg", "libs/cxxopts/include" }
        includedirs { "src" }
        links { "Static" }
        files { "tests/ray_tracing/shadow_rays_test.cpp" }

    project "Test 10. Acceleraction structure benchmark"
        kind "ConsoleApp"
        defines { "RAYTRACING" }
        includedirs { "libs/Catch2/single_include/catch2" }
        includedirs { "libs/stb", "libs/tinyobjloader", "libs/linalg", "libs/cxxopts/include" }
        includedirs { "src" }
        links { "Static" }
        files { "tests/ray_tracing/acceleraction_structure_test.cpp" }

group ""

project "03. DirectX 12"
    kind "WindowedApp"
    defines { "DX12" }
    entrypoint "WinMainCRTStartup"
    links { "d3d12", "dxgi", "d3dcompiler" }
    includedirs { "libs/stb", "libs/tinyobjloader", "libs/linalg", "libs/cxxopts/include" }
    includedirs { "libs/D3DX12", "src" }
    files { "src/settings.*"}
    files { "src/renderer/renderer.*"}
    files { "src/renderer/dx12/dx12_renderer.*"}
    files { "src/utils/resource_utils.*"}
    files { "src/world/camera.*"}
    files { "src/utils/window.*"}
    files { "src/world/model.*"}
    files {"src/win_main.cpp" }
    postbuildcommands {
       "{COPY} shaders/shaders.hlsl \"%{cfg.buildtarget.directory}\"",
    }

group "Tests"
    project "Test 11. DX12 camera"
        kind "ConsoleApp"
        defines { "DX12" }
        includedirs { "libs/Catch2/single_include/catch2" }
        includedirs { "libs/stb", "libs/tinyobjloader", "libs/linalg", "libs/cxxopts/include" }
        includedirs { "src" }
        links { "Static" }
        files { "tests/dx12/dx12_camera_test.cpp" }

group ""