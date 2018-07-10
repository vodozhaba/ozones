#pragma once

#include <cstdint>
#include <istream>
#include <vector>
#include "ram.h"

namespace ozones {

    class Rom : public MappableInterface {
    public:
        Rom(std::istream& stream, size_t size);
        uint8_t ReadByte(size_t addr) override;
        void WriteByte(size_t addr, uint8_t value) override;
    private:
        size_t size_;
        std::vector<uint8_t> contents_;
    };

}
