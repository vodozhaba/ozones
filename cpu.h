#pragma once

#include <cstdint>
#include <memory>
#include "instruction.h"
#include "ram.h"

namespace ozones {

    class Cpu {
    public:
        Cpu(std::shared_ptr<Ram> ram);
        void Tick();
    private:
        enum StatusFlag {
            kCarry              = 0x01,
            kZero               = 0x02,
            kInterruptDisable   = 0x04,
            kDecimalMode        = 0x08,
            kBrkOrPhp           = 0x10,
            kAlwaysSet          = 0x20,
            kOverflow           = 0x40,
            kNegative           = 0x80
        };
        uint8_t reg_a_, reg_x_, reg_y_, reg_sp_, reg_pc_, reg_status_;
        std::shared_ptr<Ram> ram_;
        void ExecuteInstruction(Instruction instruction);
        uint16_t OperandRead(Operand operand);
        void OperandWrite(Operand operand, uint8_t value);
        void UpdateStatus(uint8_t result);
        void SetFlag(StatusFlag flag, bool value);
        void Adc(uint8_t operand);
    };
}
