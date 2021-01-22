#pragma once

#include "renderer/renderer.h"

#include <Windows.h>

namespace cg::utils
{
class window
{
public:
	static int run(cg::renderer::renderer* renderer, HINSTANCE hinstance, int ncmdshow);
	static HWND get_hwnd() { return hwnd; }

protected:
	static LRESULT CALLBACK window_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

private:
	static HWND hwnd;
};
}


