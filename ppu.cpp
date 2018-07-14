// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "ppu.h"

namespace ozones {

Ppu::Ppu(std::shared_ptr<Ram> ppu_ram, std::shared_ptr<Ram> cpu_ram) {
    palettes_ = std::make_shared<Ram>(0x20);
    oam_ = std::make_shared<Ram>(0x100);
    ram_ = std::make_shared<Ram>(0);
    cpu_ram_ = cpu_ram;
    for(size_t i = 0; i < 4; ++i) {
        ram_->Map(ppu_ram, 0, i * 0x4000, 0x3F00);
        ram_->Map(palettes_, 0x3F00, i * 0x4000 + 0x3F00, 0x100);
    }
}

uint8_t Ppu::ReadByte(size_t addr) {
    switch(addr & 0x7) {
    case 2:
        scroll_write_pair_ = false;
        ppu_write_pair_ = false;
        return latch_ = (ppu_status_ & 0xE0) | (latch_ & 0x1F);
    case 4:
        return latch_ = oam_->ReadByte(oam_addr_);
    case 7:
        return latch_ = ram_->ReadByte(ppu_addr_);
    default:
        return latch_;
    }
}

void Ppu::WriteByte(size_t addr, uint8_t value) {
    latch_ = value;
    if(addr == 0x4014) {
        for(size_t i = 0; i < 256; i++) {
            uint8_t byte = cpu_ram_->ReadByte(((uint16_t) value << 8) + i);
            oam_->WriteByte(i, byte);
        }
    }
    switch(addr & 0x7) {
    case 0:
        ppu_ctrl_ = value;
        break;
    case 1:
        ppu_mask_ = value;
        break;
    case 3:
        if(oam_write_pair_)
            oam_addr_ = (uint16_t) value | (oam_addr_ & 0xFF00);
        else
            oam_addr_ = (uint16_t)(value << 8) | (oam_addr_ & 0x00FF);
        oam_write_pair_ = !oam_write_pair_;
        break;
    case 4:
        oam_->WriteByte(oam_addr_, value);
        if(ppu_status_ & kVBlank)
            break;
        ++oam_addr_;
        break;
    case 5:
        if(scroll_write_pair_)
            fine_scroll_y_ = value;
        else
            fine_scroll_x_ = value;
        scroll_write_pair_ = !scroll_write_pair_;
        break;
    case 6:
        if(ppu_write_pair_)
            ppu_addr_ = (uint16_t) value | (ppu_addr_ & 0xFF00);
        else
            ppu_addr_ = (uint16_t)(value << 8) | (ppu_addr_ & 0x00FF);
        ppu_write_pair_ = !ppu_write_pair_;
        break;
    case 7:
        ram_->WriteByte(ppu_addr_, value);
        if(ppu_ctrl_ & kIncrementMode)
            ppu_addr_ += 32;
        else
            ++ppu_addr_;
        break;
    }
}

}
