#pragma once

#include <cstdint>
#include <vector>

namespace ozones {

    class Ram {
    public:
        Ram(size_t size);
        uint8_t ReadByte(size_t addr);
        uint16_t ReadWord(size_t addr);
        void WriteByte(size_t addr, uint8_t value);
        void WriteWord(size_t addr, uint16_t value);
    private:
        size_t size_;
        std::vector<uint8_t> contents_;
    };

}
