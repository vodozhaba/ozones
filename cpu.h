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
        uint8_t reg_a_, reg_x_, reg_y_, reg_sp_, reg_pc_, reg_status_;
        std::shared_ptr<Ram> ram_;
        void ExecuteInstruction(Instruction instruction);
        uint16_t OperandRead(Operand operand);
    };
}
