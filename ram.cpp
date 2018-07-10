// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "ram.h"

namespace ozones {

    Ram::Ram(size_t size) : size_(size), contents_(size) { }

    uint8_t Ram::ReadByte(size_t addr) {
        for(auto& mapping : mappings_) {
            if(mapping.source_start <= addr && mapping.source_start + mapping.length > addr) {
                return mapping.destination->ReadByte(addr - mapping.source_start + mapping.dest_start);
            }
        }
        addr %= size_;
        return contents_[addr];
    }

    uint16_t Ram::ReadWord(size_t addr) {
        return ReadByte(addr) | (uint16_t) ReadByte(addr + 1) << 8;
    }

    void Ram::WriteByte(size_t addr, uint8_t value) {
        for(auto& mapping : mappings_) {
            if(mapping.source_start <= addr && mapping.source_start + mapping.length > addr) {
                mapping.destination->WriteByte(addr - mapping.source_start + mapping.dest_start, value);
                return;
            }
        }
        addr %= size_;
        contents_[addr] = value;
    }

    void Ram::WriteWord(size_t addr, uint16_t value) {
        WriteByte(addr, value);
        WriteByte(addr + 1, value >> 8);
    }

    void Ram::Map(std::shared_ptr<MappableInterface> destination, size_t source_start, size_t dest_start, size_t length) {
        mappings_.push_back(Mapping(destination, source_start, dest_start, length));
    }

    Ram::Mapping::Mapping(std::shared_ptr<MappableInterface> destination, size_t source_start, size_t dest_start, size_t length) : destination(destination), source_start(source_start), dest_start(dest_start), length(length) { }

}
