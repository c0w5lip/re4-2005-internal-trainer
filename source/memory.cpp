#include <windows.h>
#include <cstdint>

#include "memory.h"

void SetValue(ptrdiff_t address, int value) {
	const auto game_module_base_address = reinterpret_cast<std::uintptr_t>(GetModuleHandle("bio4.exe"));

	if (!game_module_base_address) {
		// TODO: MessageBox: couldn't find the game module
		return;
	}

	const auto local_player_address = *reinterpret_cast<std::uintptr_t*>(game_module_base_address + offsets::local_player_pointer);
	*reinterpret_cast<std::uintptr_t*>(local_player_address + address) = value;
}
