#pragma once

#include <cstdint>

namespace ozones {
struct INesHeader {
    uint32_t signature;
    uint8_t prg_rom_size; // in 16 KiB units
    uint8_t chr_rom_size; // in 8KiB units
    bool vertical_mirroring : 1;
    bool has_prg_ram : 1;
    bool has_trainer : 1;
    bool ignore_mirroring : 1;
    uint8_t mapper_low : 4;
    bool vs_unisystem : 1;
    bool playchoice_10 : 1;
    uint8_t format : 2;
    uint8_t mapper_high : 4;
    uint8_t various[8];
} __attribute__((packed));
}
