#include "window.h"

#include <settings.h>
#include <sstream>
#include <windowsx.h>

using namespace cg::utils;

HWND window::hwnd = nullptr;


int cg::utils::window::run(cg::renderer::renderer* renderer, HINSTANCE hinstance, int ncmdshow)
{
    // Initialize the window class.
    WNDCLASSEX window_class = {};
    window_class.cbSize = sizeof(WNDCLASSEX);
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = window_proc;
    window_class.hInstance = hinstance;
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    window_class.lpszClassName = L"DXSampleClass";
    RegisterClassEx(&window_class);

    // Create the window and store a handle to it.
    RECT window_rect = { 0, 0, static_cast<LONG>(renderer->get_width()), static_cast<LONG>(renderer->get_height()) };
    AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW, FALSE);
    hwnd = CreateWindow(
        window_class.lpszClassName, L"DX12 renderer", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        window_rect.right - window_rect.left, window_rect.bottom - window_rect.top, nullptr, nullptr, hinstance, renderer);

    // Initialize the sample. OnInit is defined in each child-implementation of DXSample.
    renderer->init();
    ShowWindow(hwnd, ncmdshow);
    // Main sample loop.
    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    renderer->destroy();
    // Return this part of the WM_QUIT message to Windows.
    return static_cast<int>(msg.wParam);
}

LRESULT cg::utils::window::window_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    cg::renderer::renderer* renderer = reinterpret_cast<cg::renderer::renderer*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    switch (message)
    {
    case WM_CREATE: {
        // Save the Renderer* passed in to CreateWindow.
        LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lparam);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
    }
        return 0;

    case WM_PAINT: {
        if (renderer)
        {
            renderer->update();
            renderer->render();
        }
    }
        return 0;

    case WM_KEYDOWN: {
        if (renderer)
        {
            switch (static_cast<UINT8>(wparam))
            {
            case 87: // w
                renderer->move_forward();
                break;
            case 83: // s
                renderer->move_backward();
                break;
            case 68: // d
                renderer->move_right();
                break;
            case 65: // a
                renderer->move_left();
                break;
            }
        }
    }
        return 0;

    case WM_MOUSEMOVE: {
        if (renderer)
        {
            // // Center the cursor back
            // RECT rect;
            // if (GetWindowRect(hwnd, &rect))
            // {
            //     const int width = rect.right - rect.left;
            //     const int height = rect.bottom - rect.top;
                
            //     POINT point;
            //     point.x = width / 2 + rect.left;
            //     point.y = height / 2 + rect.top;

            //     // short x_pos = GET_X_LPARAM(lparam);
            //     // short y_pos = GET_Y_LPARAM(lparam);

            //     POINT cursor_pos;
            //     GetCursorPos(&cursor_pos);


            //     const int delta_x = (cursor_pos.x - rect.left) - width / 2;
            //     const int delta_y = (cursor_pos.y - rect.top) - height / 2;


            //     std::stringstream ss;
            //     ss << delta_x << " " << delta_y << std::endl;
            //     OutputDebugStringA(ss.str().c_str());

            //     // printf("%i %i\n", x_pos - width / 2, y_pos - height / 2);

            //     renderer->move_yaw((2.f * static_cast<float>(delta_x) / renderer->get_width() - 1.f) * 30.f);
            //     renderer->move_pitch((-2.f * static_cast<float>(delta_y) / renderer->get_height() + 1.f) * 30.f);

            //     // ClientToScreen(hwnd, &point);
            //     SetCursorPos(point.x, point.y);
            // }


            short x_pos = GET_X_LPARAM(lparam);
            short y_pos = GET_Y_LPARAM(lparam);

            renderer->move_yaw((2.f * static_cast<float>(x_pos) / renderer->get_width() - 1.f) * 30.f);
            renderer->move_pitch((-2.f * static_cast<float>(y_pos) / renderer->get_height() + 1.f) * 30.f);
        }
    }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    // Handle any messages the switch statement didn't.
    return DefWindowProc(hwnd, message, wparam, lparam);
}
