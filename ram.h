#pragma once

#include <cstdint>
#include <memory>
#include <vector>

namespace ozones {

class Mappable {
public:
    virtual uint8_t ReadByte(size_t addr) = 0;
    virtual void WriteByte(size_t addr, uint8_t value) = 0;
    uint16_t ReadWord(size_t addr);
    void WriteWord(size_t addr, uint16_t value);
};

class Ram : public Mappable {
public:
    Ram(size_t size = 0);
    uint8_t ReadByte(size_t addr) override;
    void WriteByte(size_t addr, uint8_t value) override;
    void Map(std::shared_ptr<Mappable> destination, size_t source_start, size_t dest_start, size_t length);
private:
    struct Mapping {
        Mapping(std::shared_ptr<Mappable>, size_t source_start, size_t dest_start, size_t length);
        std::shared_ptr<Mappable> destination;
        size_t source_start;
        size_t dest_start;
        size_t length;
    };
    size_t size_;
    std::vector<uint8_t> contents_;
    std::vector<Mapping> mappings_;
};

}
