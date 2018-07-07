// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "ram.h"

namespace ozones {

    Ram::Ram(size_t size) : size_(size), contents_(size) { }

    uint8_t Ram::ReadByte(size_t addr) {
        addr %= size_;
        return contents_[addr];
    }

    uint16_t Ram::ReadWord(size_t addr) {
        addr %= size_;
        return (uint16_t) contents_[addr] | contents_[addr + 1] << 8;
    }

    void Ram::WriteByte(size_t addr, uint8_t value) {
        addr %= size_;
        contents_[addr] = value;
    }

    void Ram::WriteWord(size_t addr, uint16_t value) {
        addr %= size_;
        contents_[addr] = value;
        contents_[addr + 1] = value >> 8;
    }

}
