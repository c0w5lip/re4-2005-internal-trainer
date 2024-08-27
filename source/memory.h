#pragma once

#include <cstddef>

namespace offsets {
    constexpr std::ptrdiff_t local_player_pointer = 0x805F3C;

    constexpr std::ptrdiff_t money = 0x4FA8;
    constexpr std::ptrdiff_t health = 0x4FB4;
}

void SetValue(ptrdiff_t address, int value);
