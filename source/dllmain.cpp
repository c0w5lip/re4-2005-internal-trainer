#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <thread>

#include <d3d9.h>
#include <d3dx9.h>
#include <detours.h>

#include "gui.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "detours.lib")


typedef HRESULT(_stdcall* EndScene)(LPDIRECT3DDEVICE9 pDevice);
HRESULT _stdcall hookEndScene(LPDIRECT3DDEVICE9 pDevice);


namespace variables {
    static LPDIRECT3DDEVICE9 PD3D_DEVICE = nullptr;
    static IDirect3D9* PD3D = nullptr;
    static WNDPROC WNDPROC_ORIGNAL = nullptr;
    static HWND window = nullptr;
    static bool isInit = false;
    static EndScene oEndScene;
}



LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (gui::isMenuToggled && ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
        return true;
    }

    return CallWindowProc(variables::WNDPROC_ORIGNAL, hWnd, msg, wParam, lParam);
}



BOOL CALLBACK EnumWidnowsCallback(HWND handle, LPARAM lParam) {
    DWORD wndProcID;
    GetWindowThreadProcessId(handle, &wndProcID);

    if (GetCurrentProcessId() != wndProcID)
    {
        return true;
    }

    variables::window = handle;
    return false;
}

HWND GetProcessWindow() {
    EnumWindows(EnumWidnowsCallback, NULL);
    return variables::window;
}




bool GetD3D9Device(void** pTable, size_t size)
{
    if (!pTable)
        return false;

    // Create a D3D Variable and get the sdk version
    variables::PD3D = Direct3DCreate9(D3D_SDK_VERSION);

    // Make sure that the pointer is valid
    if (!variables::PD3D)
        return false;

    D3DPRESENT_PARAMETERS d3dpp = {};
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    // The game window we want to render on
    d3dpp.hDeviceWindow = GetProcessWindow();
    d3dpp.Windowed = true;

    variables::PD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &variables::PD3D_DEVICE);

    if (!variables::PD3D_DEVICE) {
        variables::PD3D->Release();
        return false;
    }

    // We are copying the pTable that we get from the device and its gonna be the size of the pTable
    memcpy(pTable, *reinterpret_cast<void***>(variables::PD3D_DEVICE), size);

    // Realase everything at the end
    variables::PD3D_DEVICE->Release();
    variables::PD3D->Release();

    return true;
}


HRESULT __stdcall hookEndScene(LPDIRECT3DDEVICE9 pDevice)
{
    if (!variables::isInit) {
        // Call the original game message handling fnc
        variables::WNDPROC_ORIGNAL = (WNDPROC)SetWindowLongPtr(variables::window, GWLP_WNDPROC, (LONG_PTR)WndProc);

        // Draw the ImGui Menu
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();

        ImGui::StyleColorsDark();
        ImGui_ImplWin32_Init(variables::window);
        ImGui_ImplDX9_Init(pDevice);

        // Init to true to prevent spamming the message box
        variables::isInit = true;
    }

    if (GetAsyncKeyState('L') & 1) { // ;)
        gui::isMenuToggled = !gui::isMenuToggled;
        Sleep(1);
    }

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (gui::isMenuToggled) {
        ImGui::Begin("Faces Menu", &gui::isMenuToggled);
        static bool isChamsToggled = false;
        ImGui::Checkbox("Chams", &isChamsToggled);
    }

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());


    return variables::oEndScene(pDevice);
}

void MainThread(const HMODULE instance) noexcept {
    void* d3d9Device[119];

    if (GetD3D9Device(d3d9Device, sizeof(d3d9Device))) {
        variables::oEndScene = (EndScene)d3d9Device[42];
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)variables::oEndScene, hookEndScene);
        DetourTransactionCommit();


        while (!GetAsyncKeyState(VK_END)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&(PVOID&)variables::oEndScene, hookEndScene);
        DetourTransactionCommit();


        if (variables::PD3D_DEVICE) {
            variables::PD3D_DEVICE->Release();
            variables::PD3D_DEVICE = nullptr;
        }

        if (variables::PD3D) {
            variables::PD3D->Release();
            variables::PD3D = nullptr;
        }
    }

	FreeLibraryAndExitThread(instance, 0);
}

int __stdcall DllMain(const HMODULE instance, const std::uintptr_t reason, const void* reserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(instance);


		const auto thread = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE> (MainThread), instance, 0, nullptr);

		if (thread) {
			CloseHandle(thread);
		}
	}

	return 1;
}
