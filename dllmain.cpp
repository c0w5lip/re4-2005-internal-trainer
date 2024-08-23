#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <thread>


namespace offsets {
	constexpr std::ptrdiff_t local_player_pointer = 0x805F3C;

	constexpr std::ptrdiff_t pesetas = 0x4FA8;
}


void MainThread(const HMODULE instance) noexcept {
	const auto game_module_base_address = reinterpret_cast<std::uintptr_t>(GetModuleHandle("bio4.exe"));

	if (!game_module_base_address) {
		return;
	}

	while (!GetAsyncKeyState(VK_END)) {
		if (GetAsyncKeyState('L')) {

			const auto local_player_address = *reinterpret_cast<std::uintptr_t*>(game_module_base_address + offsets::local_player_pointer);
			*reinterpret_cast<std::uintptr_t*>(local_player_address + offsets::pesetas) = 1337;

		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
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
