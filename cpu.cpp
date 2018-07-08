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
        case Instruction::kNop:
            break;
        case Instruction::kOra:
            reg_a_ |= OperandRead(instruction.GetOperand());
            UpdateStatus(reg_a_);
            break;
        case Instruction::kAnd:
            reg_a_ &= OperandRead(instruction.GetOperand());
            UpdateStatus(reg_a_);
            break;
        case Instruction::kEor: // Seriously? You don't know the word XOR?
            reg_a_ ^= OperandRead(instruction.GetOperand());
            UpdateStatus(reg_a_);
            break;
        case Instruction::kAdc: {
            uint8_t operand = OperandRead(instruction.GetOperand());
            Adc(operand);
            break;
        }
        case Instruction::kSta:
            OperandWrite(instruction.GetOperand(), reg_a_);
            break;
        case Instruction::kLda:
            reg_a_ = OperandRead(instruction.GetOperand());
            break;
        case Instruction::kCmp: {
            uint8_t operand = OperandRead(instruction.GetOperand());
            SetFlag(kNegative, reg_a_ < operand);
            SetFlag(kZero, reg_a_ == operand);
            SetFlag(kCarry, reg_a_ > operand);
            break;
        }
        case Instruction::kSbc: {
            // A + M = A + (-M)
            uint8_t operand = OperandRead(instruction.GetOperand());
            Adc(~operand);
            break;
        }
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
            throw new std::runtime_error("Attempted to read an implied operand");
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
        default:
            std::stringstream ss;
            ss << "Unknown addressing mode: " << operand.GetMode();
            throw std::runtime_error(ss.str());
        }
    }

    void Cpu::OperandWrite(Operand operand, uint8_t value) {
        switch(operand.GetMode()) {
        case Operand::kImmediate:
            throw new std::runtime_error("Attemped to write to an immediate value");
        case Operand::kAbsolute:
            ram_->WriteByte(operand.GetValue(), value);
            break;
        case Operand::kImplied:
            throw new std::runtime_error("Attempted to write to an implied operand");
        case Operand::kAbsoluteIndexedX:
            ram_->WriteByte(operand.GetValue() + reg_x_, value);
            break;
        case Operand::kAbsoluteIndexedY:
            ram_->WriteByte(operand.GetValue() + reg_y_, value);
            break;
        // Spot the difference
        case Operand::kIndexedIndirect:
            ram_->WriteByte(ram_->ReadWord(operand.GetValue() + reg_x_), value);
            break;
        case Operand::kIndirectIndexed:
            ram_->WriteByte(ram_->ReadWord(operand.GetValue()) + reg_y_, value);
            break;
        case Operand::kRelative:
            throw new std::runtime_error("Attempted to write to a relative operand");
        case Operand::kAccumulator:
            reg_a_ = value;
            break;
        default:
            std::stringstream ss;
            ss << "Unknown addressing mode: " << operand.GetMode();
            throw std::runtime_error(ss.str());
        }
    }

    void Cpu::UpdateStatus(uint8_t result) {
        SetFlag(kZero, result == 0x00);
        SetFlag(kNegative, result & 0x80);
    }

    void Cpu::SetFlag(StatusFlag flag, bool value) {
        if(value)
            reg_sp_ |= flag;
        else
            reg_sp_ &= ~flag;
    }

    void Cpu::Adc(uint8_t operand) {
        uint8_t old_a = reg_a_;
        reg_a_ += operand;
        if(reg_status_ | kCarry)
            ++reg_a_;
        UpdateStatus(reg_a_);
        SetFlag(kCarry, old_a < reg_a_);
        // If the operands have the same sign and the result has a different one
        SetFlag(kOverflow, ((old_a & 0x80) == (operand & 0x80)) && ((old_a & 0x80) != (reg_a_ & 0x80)));
    }


}
