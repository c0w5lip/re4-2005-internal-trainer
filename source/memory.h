#pragma once

#include <cstddef>

namespace offsets {
    constexpr std::ptrdiff_t local_player_pointer = 0x805F3C;

    constexpr std::ptrdiff_t pesetas = 0x4FA8;
}

void setHealth(int value);
