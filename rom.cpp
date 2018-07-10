// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "rom.h"

namespace ozones {

    Rom::Rom(std::istream &stream, size_t size) : size_(size), contents_(size) {
        size_ = size;
        for(size_t i = 0; i < size; i++) {
            int b = stream.get();
            if(b == EOF)
                throw new std::runtime_error("Unexpected EOF in ROM");
            else
                contents_[i] = b;
        }
    }

    uint8_t Rom::ReadByte(size_t addr) {
        return contents_[addr % size_];
    }

    void Rom::WriteByte(size_t addr, uint8_t value) {
        return;
    }

}
