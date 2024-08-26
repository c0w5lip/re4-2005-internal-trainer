#pragma once

#include <windows.h>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx9.h>

namespace gui {
	static bool isMenuToggled = false;
	static bool isInit = false;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
