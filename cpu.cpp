// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "cpu.h"
#include <sstream>

namespace ozones {

    Cpu::Cpu(std::shared_ptr<Ram> ram) : reg_a_(0), reg_x_(0), reg_y_(0), reg_sp_(0xFD), reg_pc_(0), reg_status_(0x34), ram_(ram) { }

    void Cpu::Tick() {
        Instruction instr(ram_, reg_pc_);
        reg_pc_ += instr.GetLength();
        ExecuteInstruction(instr);
    }

    void Cpu::ExecuteInstruction(Instruction instruction) {
        switch(instruction.GetMnemonic()) {
        case Instruction::kOra:
            reg_a_ |= OperandRead(instruction.GetOperand(0));
            break;
        default:
            std::stringstream ss;
            ss << "Unknown instruction: " << instruction.GetMnemonic();
            throw std::runtime_error(ss.str());
        }
    }

    uint16_t Cpu::OperandRead(Operand operand) {
        switch(operand.GetMode()) {
        case Operand::kImmediate:
            return operand.GetValue();
        case Operand::kAbsolute:
            return ram_->ReadByte(operand.GetValue());
        case Operand::kImplied:
            throw new std::runtime_error("Attempted to read implied operand");
        case Operand::kIndirectAbsolute:
            return ram_->ReadWord(operand.GetValue());
        case Operand::kAbsoluteIndexedX:
            return ram_->ReadByte(operand.GetValue() + reg_x_);
        case Operand::kAbsoluteIndexedY:
            return ram_->ReadByte(operand.GetValue() + reg_y_);
            // Spot the difference
        case Operand::kIndexedIndirect:
            return ram_->ReadByte(ram_->ReadWord(operand.GetValue() + reg_x_));
        case Operand::kIndirectIndexed:
            return ram_->ReadByte(ram_->ReadWord(operand.GetValue()) + reg_y_);
        case Operand::kRelative:
            return reg_pc_ + (int8_t) operand.GetValue();
        case Operand::kAccumulator:
            return reg_a_;
        }
        std::stringstream ss;
        ss << "Unknown addressing mode: " << operand.GetMode();
        throw std::runtime_error(ss.str());
    }

}
