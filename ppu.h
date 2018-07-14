#pragma once

#include <cstdint>
#include <memory>
#include "ram.h"

namespace ozones {

class Ppu : public Mappable {
public:
    enum CtrlFlags {
        kNametableSelect        = 0x03,
        kIncrementMode          = 0x04,
        kSpriteTileSelect       = 0x08,
        kBackgroundTileSelect   = 0x10,
        kSpriteHeight           = 0x20,
        kPpuMasterSlave         = 0x40,
        kNmiEnable              = 0x80
    };
    enum MaskFlags {
        kGreyscale                  = 0x01,
        kBackgroundLeftColumnEnable = 0x02,
        kSpriteLeftColumnEnable     = 0x04,
        kBackgroundEnable           = 0x08,
        kSpriteEnable               = 0x10,
        kEmphasisRed                = 0x20,
        kEmphasisGreen              = 0x40,
        kEmphasisBlue               = 0x80
    };
    enum StatusFlags {
        kSpriteOverflow = 0x20,
        kSpriteZeroHit  = 0x40,
        kVBlank         = 0x80
    };
    Ppu(std::shared_ptr<Ram> ppu_ram, std::shared_ptr<Ram> cpu_ram);
    uint8_t ReadByte(size_t addr) override;
    void WriteByte(size_t addr, uint8_t value) override;
private:
    std::shared_ptr<Ram> palettes_;
    std::shared_ptr<Ram> oam_;
    std::shared_ptr<Ram> ram_;
    std::shared_ptr<Ram> cpu_ram_;
    uint8_t latch_;
    uint8_t ppu_ctrl_, ppu_mask_, ppu_status_, oam_dma_;
    uint16_t ppu_addr_, oam_addr_;
    uint8_t fine_scroll_x_, fine_scroll_y_;
    bool oam_write_pair_, scroll_write_pair_, ppu_write_pair_;
};

}
