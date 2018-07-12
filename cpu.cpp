// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "cpu.h"
#include "unistd.h"
#include <iostream>
#include <sstream>

namespace ozones {

Cpu::Cpu(std::shared_ptr<Ram> ram) : reg_a_(0), reg_x_(0), reg_y_(0), reg_sp_(0xFD), reg_p_(0x24), reg_pc_(0xC000), cycle_counter_(0), ram_(ram), nmi_pending_(false) { }

void Cpu::Tick() {
    Instruction instr(ram_, reg_pc_);
    std::cout << std::hex << reg_pc_;
    for(size_t i = 0; i < instr.GetLength(); i++) {
        std::cout << std::hex << " " << (int) ram_->ReadByte(reg_pc_ + i);
    }
    std::cout << std::hex << " A:" << (int) reg_a_ << " X:" << (int) reg_x_ << " Y:" << (int) reg_y_ << " P:" << (int) reg_p_ << " SP:" << (int) reg_sp_;
    std::cout << std::dec << " CYC: " << cycle_counter_ << std::endl;
    TakeCycles(instr.GetCycles());
    reg_pc_ += instr.GetLength();
    ExecuteInstruction(instr);
    if(nmi_pending_)
        TriggerNmi();
    if(!(reg_p_ & kInterruptDisable) && irq_pending_)
        TriggerIrq();
}

void Cpu::SetNmiPending(bool nmi_pending) {
    nmi_pending_ = nmi_pending;
}

void Cpu::SetIrqPending(bool irq_pending) {
    irq_pending_ = irq_pending;
}

void Cpu::ExecuteInstruction(Instruction instruction) {
    switch(instruction.GetMnemonic()) {
    // Official
    case Instruction::kNop:
        if(instruction.GetOperand().GetMode() != Operand::kImplied)
            OperandRead(instruction.GetOperand());
        break;
    case Instruction::kBrk:
        PushWord(reg_pc_);
        PushByte(reg_p_ | kBrkOrPhp);
        reg_pc_ = ram_->ReadWord(0xFFFE);
        break;
    case Instruction::kPhp:
        PushByte(reg_p_ | kBrkOrPhp);
        break;
    case Instruction::kBpl:
        if(!(reg_p_ & kNegative)) {
            TakeCycles(1);
            reg_pc_ = OperandRead(instruction.GetOperand());
        }
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
        reg_p_ = PullByte();
        SetFlag(kAlwaysSet, true);
        SetFlag(kBrkOrPhp, false);
        break;
    case Instruction::kBmi: {
        if(reg_p_ & kNegative) {
            TakeCycles(1);
            reg_pc_ = OperandRead(instruction.GetOperand());
        }
        break;
    }
    case Instruction::kSec:
        SetFlag(kCarry, true);
        break;
    case Instruction::kRti:
        reg_p_ = PullByte();
        reg_pc_ = PullWord();
        SetFlag(kAlwaysSet, true);
        SetFlag(kBrkOrPhp, false);
        break;
    case Instruction::kPha:
        PushByte(reg_a_);
        break;
    case Instruction::kJmp:
        reg_pc_ = OperandRead(instruction.GetOperand());
        break;
    case Instruction::kBvc:
        if(!(reg_p_ & kOverflow)) {
            TakeCycles(1);
            reg_pc_ = OperandRead(instruction.GetOperand());
        }
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
        if(reg_p_ & kOverflow) {
            TakeCycles(1);
            reg_pc_ = OperandRead(instruction.GetOperand());
        }
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
        if(!(reg_p_ & kCarry)) {
            TakeCycles(1);
            reg_pc_ = OperandRead(instruction.GetOperand());
        }
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
        if(reg_p_ & kCarry) {
            TakeCycles(1);
            reg_pc_ = OperandRead(instruction.GetOperand());
        }
        break;
    case Instruction::kClv:
        SetFlag(kOverflow, false);
        break;
    case Instruction::kCpy: {
        uint8_t operand = OperandRead(instruction.GetOperand());
        SetFlag(kZero, reg_y_ == operand);
        SetFlag(kCarry, reg_y_ >= operand);
        SetFlag(kNegative, (reg_y_ - operand) & 0x80);
        break;
    }
    case Instruction::kIny:
        UpdateStatus(++reg_y_);
        break;
    case Instruction::kBne:
        if(!(reg_p_ & kZero)) {
            TakeCycles(1);
            reg_pc_ = OperandRead(instruction.GetOperand());
        }
        break;
    case Instruction::kCld:
        SetFlag(kDecimalMode, false);
        break;
    case Instruction::kCpx: {
        uint8_t operand = OperandRead(instruction.GetOperand());
        SetFlag(kZero, reg_x_ == operand);
        SetFlag(kCarry, reg_x_ >= operand);
        SetFlag(kNegative, (reg_x_ - operand) & 0x80);
        break;
    }
    case Instruction::kInx:
        UpdateStatus(++reg_x_);
        break;
    case Instruction::kBeq:
        if(reg_p_ & kZero) {
            TakeCycles(1);
            reg_pc_ = OperandRead(instruction.GetOperand());
        }
        break;
    case Instruction::kSed:
        SetFlag(kDecimalMode, true);
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
        UpdateStatus(reg_a_);
        break;
    case Instruction::kCmp: {
        uint8_t operand = OperandRead(instruction.GetOperand());
        SetFlag(kZero, reg_a_ == operand);
        SetFlag(kCarry, reg_a_ >= operand);
        SetFlag(kNegative, (reg_a_ - operand) & 0x80);
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
        uint8_t old_carry = (reg_p_ & kCarry) ? 1 : 0;
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
        uint8_t old_carry = (reg_p_ & kCarry) ? 0x80 : 0;
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
    }
    case Instruction::kInc: {
        uint8_t operand = OperandRead(instruction.GetOperand());
        UpdateStatus(++operand);
        OperandWrite(instruction.GetOperand(), operand);
        break;
    }
        // Unofficial
    case Instruction::kSlo: {
        uint8_t operand = OperandRead(instruction.GetOperand());
        SetFlag(kCarry, operand & 0x80);
        operand <<= 1;
        OperandWrite(instruction.GetOperand(), operand);
        reg_a_ |= operand;
        UpdateStatus(reg_a_);
        break;
    }
    case Instruction::kRla: {
        uint8_t operand = OperandRead(instruction.GetOperand());
        uint8_t old_carry = (reg_p_ & kCarry) ? 1 : 0;
        SetFlag(kCarry, operand & 0x80);
        operand <<= 1;
        operand += old_carry;
        OperandWrite(instruction.GetOperand(), operand);
        reg_a_ &= operand;
        UpdateStatus(reg_a_);
        break;
    }
    case Instruction::kSre: {
        uint8_t operand = OperandRead(instruction.GetOperand());
        SetFlag(kCarry, operand & 0x01);
        operand >>= 1;
        OperandWrite(instruction.GetOperand(), operand);
        reg_a_ ^= operand;
        UpdateStatus(reg_a_);
        break;
    }
    case Instruction::kRra: {
        uint8_t operand = OperandRead(instruction.GetOperand());
        uint8_t old_carry = (reg_p_ & kCarry) ? 0x80 : 0;
        SetFlag(kCarry, operand & 0x01);
        operand >>= 1;
        operand += old_carry;
        OperandWrite(instruction.GetOperand(), operand);
        Adc(operand);
        break;

    }
    case Instruction::kSax:
        OperandWrite(instruction.GetOperand(), reg_a_ & reg_x_);
        break;
    case Instruction::kLax:
        reg_a_ = OperandRead(instruction.GetOperand());
        reg_x_ = reg_a_;
        UpdateStatus(reg_x_);
        break;
    case Instruction::kDcp: {
        uint8_t operand = OperandRead(instruction.GetOperand());
        OperandWrite(instruction.GetOperand(), --operand);
        SetFlag(kZero, reg_a_ == operand);
        SetFlag(kCarry, reg_a_ >= operand);
        SetFlag(kNegative, (reg_a_ - operand) & 0x80);
        break;
    }
    case Instruction::kIsc: {
        uint8_t operand = OperandRead(instruction.GetOperand());
        OperandWrite(instruction.GetOperand(), ++operand);
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
        if((operand.GetValue() & 0xFF) + reg_x_ > 0xFF && operand.HasPageBoundaryPenalty()) {
            TakeCycles(1);
        }
        return ram_->ReadByte((operand.GetValue() + reg_x_) & 0xFFFF);
    case Operand::kAbsoluteIndexedY:
        if((operand.GetValue() & 0xFF) + reg_y_ > 0xFF && operand.HasPageBoundaryPenalty()) {
            TakeCycles(1);
        }
        return ram_->ReadByte((operand.GetValue() + reg_y_) & 0xFFFF);
    case Operand::kZeroPageIndexedX:
        return ram_->ReadByte((operand.GetValue() + reg_x_) & 0xFF);
    case Operand::kZeroPageIndexedY:
        return ram_->ReadByte((operand.GetValue() + reg_y_) & 0xFF);
        // Spot the difference
    case Operand::kIndexedIndirect: {
        uint16_t lsb = ram_->ReadByte((operand.GetValue() + reg_x_) & 0xFF);
        uint16_t msb = ram_->ReadByte((operand.GetValue() + reg_x_ + 1) & 0xFF);
        return ram_->ReadByte(lsb | (msb << 8));
    }
    case Operand::kIndirectIndexed: {
        uint16_t lsb = ram_->ReadByte(operand.GetValue() & 0xFF);
        uint16_t msb = ram_->ReadByte((operand.GetValue() + 1) & 0xFF);
        uint16_t addr = (lsb | (msb << 8)) + reg_y_;
        if((addr & 0xFF00) != (msb << 8) && operand.HasPageBoundaryPenalty())
            TakeCycles(1);
        return ram_->ReadByte(addr);
    }
    case Operand::kRelative: {
        uint16_t new_reg_pc = reg_pc_ + (int8_t) operand.GetValue();
        if((new_reg_pc & 0xFF00) != (reg_pc_ & 0xFF00) && operand.HasPageBoundaryPenalty()) {
            TakeCycles(1);
        }
        return new_reg_pc;
    }
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
        if((operand.GetValue() & 0xFF) + reg_x_ > 0xFF && operand.HasPageBoundaryPenalty()) {
            TakeCycles(1);
        }
        ram_->WriteByte((operand.GetValue() + reg_x_) & 0xFFFF, value);
        break;
    case Operand::kAbsoluteIndexedY:
        if((operand.GetValue() & 0xFF) + reg_y_ > 0xFF && operand.HasPageBoundaryPenalty()) {
            TakeCycles(1);
        }
        ram_->WriteByte((operand.GetValue() + reg_y_) & 0xFFFF, value);
        break;
    case Operand::kZeroPageIndexedX:
        ram_->WriteByte((operand.GetValue() + reg_x_) & 0xFF, value);
        break;
    case Operand::kZeroPageIndexedY:
        ram_->WriteByte((operand.GetValue() + reg_y_) & 0xFF, value);
        break;
        // Spot the difference
    case Operand::kIndexedIndirect: {
        uint16_t lsb = ram_->ReadByte((operand.GetValue() + reg_x_) & 0xFF);
        uint16_t msb = ram_->ReadByte((operand.GetValue() + reg_x_ + 1) & 0xFF);
        ram_->WriteByte(lsb | (msb << 8), value);
        break;
    }
    case Operand::kIndirectIndexed: {
        uint16_t lsb = ram_->ReadByte(operand.GetValue()) & 0xFF;
        uint16_t msb = ram_->ReadByte(operand.GetValue() + 1) & 0xFF;
        uint16_t addr = (lsb | (msb << 8)) + reg_y_;
        if((addr & 0xFF00) != (msb << 8) && operand.HasPageBoundaryPenalty())
            TakeCycles(1);
        ram_->WriteByte(addr, value);
        break;
    }
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
        reg_p_ |= flag;
    else
        reg_p_ &= ~flag;
}

void Cpu::Adc(uint8_t operand) {
    uint16_t a = reg_a_;
    a += operand;
    if(reg_p_ & kCarry)
        ++a;
    SetFlag(kCarry, a & 0x100);
    // If the operands have the same sign and the result has a different one
    SetFlag(kOverflow, ((reg_a_ & 0x80) == (operand & 0x80)) && ((reg_a_ & 0x80) != (a & 0x80)));
    reg_a_ = a;
    UpdateStatus(reg_a_);
}

void Cpu::PushByte(uint8_t value) {
    ram_->WriteByte(0x100 + reg_sp_--, value);
}

void Cpu::PushWord(uint16_t value) {
    --reg_sp_;
    ram_->WriteWord(0x100 + reg_sp_--, value);
}

uint8_t Cpu::PullByte() {
    return ram_->ReadByte(0x100 + ++reg_sp_);
}

uint16_t Cpu::PullWord() {
    ++reg_sp_;
    return ram_->ReadWord(0x100 + reg_sp_++);
}

void Cpu::TakeCycles(int n) {
    cycle_counter_ += 3 * n;
    cycle_counter_ %= 341;
}

void Cpu::TriggerNmi() {
    PushWord(reg_pc_);
    PushByte(reg_p_);
    reg_pc_ = ram_->ReadWord(0xFFFA);
    nmi_pending_ = false;
}

void Cpu::TriggerIrq() {
    PushWord(reg_pc_);
    PushByte(reg_p_);
    SetFlag(kInterruptDisable, true);
    reg_pc_ = ram_->ReadWord(0xFFFE);
}

}
