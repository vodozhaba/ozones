// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "cpu.h"
#include <sstream>

namespace ozones {

    Cpu::Cpu(std::shared_ptr<Ram> ram) : reg_a_(0), reg_x_(0), reg_y_(0), reg_sp_(0xFD), reg_sr_(0x34), reg_pc_(0), ram_(ram) { }

    void Cpu::Tick() {
        Instruction instr(ram_, reg_pc_);
        reg_pc_ += instr.GetLength();
        ExecuteInstruction(instr);
    }

    void Cpu::ExecuteInstruction(Instruction instruction) {
        switch(instruction.GetMnemonic()) {
        case Instruction::kNop:
            break;
        case Instruction::kPhp:
            PushByte(reg_sr_);
            break;
        case Instruction::kBpl:
            if(!(reg_sr_ & kNegative))
                reg_pc_ = OperandRead(instruction.GetOperand());
            break;
        case Instruction::kClc:
            SetFlag(kCarry, false);
            break;
        case Instruction::kJsr:
            PushWord(reg_pc_ - 1);
            reg_pc_ = OperandRead(instruction.GetOperand());
            break;
        case Instruction::kBit: {
            uint8_t operand = OperandRead(instruction.GetOperand());
            SetFlag(kNegative, operand & 0x80);
            SetFlag(kOverflow, operand & 0x40);
            SetFlag(kZero, (operand & reg_a_) == 0);
            break;
        }
        case Instruction::kPlp:
            reg_sr_ = PullWord();
            break;
        case Instruction::kBmi: {
            if(reg_sr_ & kNegative)
                reg_pc_ = OperandRead(instruction.GetOperand());
            break;
        }
        case Instruction::kSec:
            SetFlag(kCarry, true);
            break;
        case Instruction::kRti:
            reg_sr_ = PullByte();
            reg_pc_ = PullWord();
            break;
        case Instruction::kPha:
            PushByte(reg_a_);
            break;
        case Instruction::kJmp:
            reg_pc_ = OperandRead(instruction.GetOperand());
            break;
        case Instruction::kBvc:
            if(!(reg_sr_ & kOverflow))
                reg_pc_ = OperandRead(instruction.GetOperand());
            break;
        case Instruction::kCli:
            SetFlag(kInterruptDisable, false);
            break;
        case Instruction::kRts:
            reg_pc_ = PullWord() + 1;
            break;
        case Instruction::kPla:
            reg_a_ = PullByte();
            UpdateStatus(reg_a_);
            break;
        case Instruction::kBvs:
            if(reg_sr_ & kOverflow)
                reg_pc_ = OperandRead(instruction.GetOperand());
            break;
        case Instruction::kSei:
            SetFlag(kInterruptDisable, true);
            break;
        case Instruction::kSty:
            OperandWrite(instruction.GetOperand(), reg_y_);
            break;
        case Instruction::kDey:
            UpdateStatus(--reg_y_);
            break;
        case Instruction::kBcc:
            if(!(reg_sr_ & kCarry))
                reg_pc_ = OperandRead(instruction.GetOperand());
            break;
        case Instruction::kTya:
            reg_a_ = reg_y_;
            UpdateStatus(reg_a_);
            break;
        case Instruction::kLdy:
            reg_y_ = OperandRead(instruction.GetOperand());
            UpdateStatus(reg_y_);
            break;
        case Instruction::kTay:
            reg_y_ = reg_a_;
            UpdateStatus(reg_y_);
            break;
        case Instruction::kBcs:
            if(reg_sr_ & kCarry)
                reg_pc_ = OperandRead(instruction.GetOperand());
            break;
        case Instruction::kClv:
            SetFlag(kOverflow, false);
            break;
        case Instruction::kCpy: {
            uint8_t operand = OperandRead(instruction.GetOperand());
            SetFlag(kNegative, reg_y_ < operand);
            SetFlag(kZero, reg_y_ == operand);
            SetFlag(kCarry, reg_y_  > operand);
            break;
        }
        case Instruction::kIny:
            UpdateStatus(++reg_y_);
            break;
        case Instruction::kBne:
            if(!(reg_sr_ & kZero))
                reg_pc_ = OperandRead(instruction.GetOperand());
            break;
        case Instruction::kCld:
            SetFlag(kDecimalMode, true);
            break;
        case Instruction::kCpx: {
            uint8_t operand = OperandRead(instruction.GetOperand());
            SetFlag(kNegative, reg_x_ < operand);
            SetFlag(kZero, reg_x_ == operand);
            SetFlag(kCarry, reg_x_  > operand);
            break;
        }
        case Instruction::kInx:
            UpdateStatus(++reg_x_);
            break;
        case Instruction::kBeq:
            if(reg_sr_ & kZero)
                reg_pc_ = OperandRead(instruction.GetOperand());
            break;
        case Instruction::kSed:
            SetFlag(kDecimalMode, true);
            throw new std::runtime_error("Decimal mode is currently not supported");
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
        case Instruction::kAsl: {
            uint8_t operand = OperandRead(instruction.GetOperand());
            SetFlag(kCarry, operand & 0x80);
            operand <<= 1;
            OperandWrite(instruction.GetOperand(), operand);
            UpdateStatus(operand);
            break;
        }
        case Instruction::kRol: {
            uint8_t operand = OperandRead(instruction.GetOperand());
            uint8_t old_carry = (reg_sr_ & kCarry) ? 1 : 0;
            SetFlag(kCarry, operand & 0x80);
            operand <<= 1;
            operand += old_carry;
            OperandWrite(instruction.GetOperand(), operand);
            UpdateStatus(operand);
            break;
        }
        case Instruction::kLsr: {
            uint8_t operand = OperandRead(instruction.GetOperand());
            SetFlag(kCarry, operand & 0x01);
            operand >>= 1;
            OperandWrite(instruction.GetOperand(), operand);
            UpdateStatus(operand);
            break;
        }
        case Instruction::kRor: {
            uint8_t operand = OperandRead(instruction.GetOperand());
            uint8_t old_carry = (reg_sr_ & kCarry) ? 0x80 : 0;
            SetFlag(kCarry, operand & 0x01);
            operand >>= 1;
            operand += old_carry;
            OperandWrite(instruction.GetOperand(), operand);
            UpdateStatus(operand);
            break;
        }
        case Instruction::kStx:
            OperandWrite(instruction.GetOperand(), reg_x_);
            break;
        case Instruction::kTxa:
            reg_a_ = reg_x_;
            UpdateStatus(reg_a_);
            break;
        case Instruction::kTxs:
            reg_sp_ = reg_x_;
            break;
        case Instruction::kLdx:
            reg_x_ = OperandRead(instruction.GetOperand());
            UpdateStatus(reg_x_);
            break;
        case Instruction::kTax:
            reg_x_ = reg_a_;
            UpdateStatus(reg_x_);
            break;
        case Instruction::kTsx:
            reg_x_ = reg_sp_;
            UpdateStatus(reg_x_);
            break;
        case Instruction::kDec: {
            uint8_t operand = OperandRead(instruction.GetOperand());
            UpdateStatus(--operand);
            OperandWrite(instruction.GetOperand(), operand);
            break;
        }
        case Instruction::kDex: {
            UpdateStatus(--reg_x_);
            break;
        case Instruction::kInc: {
                uint8_t operand = OperandRead(instruction.GetOperand());
                UpdateStatus(++operand);
                OperandWrite(instruction.GetOperand(), operand);
                break;
            }
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
        case Operand::kZeroPageIndexedX:
            return ram_->ReadByte((operand.GetValue() + reg_x_) & 0xFF);
        case Operand::kZeroPageIndexedY:
            return ram_->ReadByte((operand.GetValue() + reg_y_) & 0xFF);
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
        case Operand::kZeroPageIndexedX:
            ram_->WriteByte((operand.GetValue() + reg_x_) & 0xFF, value);
            break;
        case Operand::kZeroPageIndexedY:
            ram_->WriteByte((operand.GetValue() + reg_y_) & 0xFF, value);
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
        if(reg_sr_ | kCarry)
            ++reg_a_;
        UpdateStatus(reg_a_);
        SetFlag(kCarry, old_a < reg_a_);
        // If the operands have the same sign and the result has a different one
        SetFlag(kOverflow, ((old_a & 0x80) == (operand & 0x80)) && ((old_a & 0x80) != (reg_a_ & 0x80)));
    }

    void Cpu::PushByte(uint8_t value) {
        ram_->WriteByte(reg_sp_--, value);
    }

    void Cpu::PushWord(uint16_t value) {
        ram_->WriteByte(reg_sp_, value);
        reg_sp_ -= 2;
    }

    uint8_t Cpu::PullByte() {
        return ram_->ReadByte(++reg_sp_);
    }

    uint16_t Cpu::PullWord() {
        reg_sp_ += 2;
        return ram_->ReadWord(reg_sp_);
    }

}
