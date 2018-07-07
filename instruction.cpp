// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "instruction.h"
#include <sstream>

namespace ozones {

    Operand::Operand(Operand::AddressingMode mode, uint16_t addr) : mode_(mode), value_(addr) { }

    Operand::AddressingMode Operand::GetMode() {
        return mode_;
    }

    uint16_t Operand::GetValue() {
        return value_;
    }

    Instruction::Instruction(std::shared_ptr<Ram> ram, uint16_t addr) {
        // Operation
        uint8_t opcode = ram->ReadByte(addr);
        switch(opcode & 0xF3) {
        case 0x01:
            mnemonic_ = kOra;
            break;
        case 0x21:
            mnemonic_ = kAnd;
            break;
        case 0x41:
            mnemonic_ = kEor;
            break;
        case 0x61:
            mnemonic_ = kAdc;
            break;
        case 0x81:
            if(opcode == 0x89)
                mnemonic_ = kNop;
            else
                mnemonic_ = kSta;
            break;
        case 0xA1:
            mnemonic_ = kLda;
            break;
        case 0xC1:
            mnemonic_ = kCmp;
            break;
        case 0xE1:
            mnemonic_ = kSbc;
            break;
        default:
            std::stringstream ss;
            ss << "Unknown opcode: 0x" << std::hex << opcode;
            throw std::runtime_error(ss.str());
        }
        // Operands
        ++addr;
        uint16_t data_addr;
        switch(opcode & 0x0C) {
        case 0x00: // (aa, X)
            data_addr = ram->ReadByte(addr);
            operands_.push_back((Operand(Operand::kIndexedIndirect, data_addr)));
            length_ = 2;
            break;
        case 0x04: // aa
            data_addr = ram->ReadByte(addr);
            operands_.push_back((Operand(Operand::kAbsolute, data_addr)));
            length_ = 2;
            break;
        case 0x08: // #aa
            data_addr = ram->ReadByte(addr); // Not really an address but an immediate value
            operands_.push_back((Operand(Operand::kImmediate, data_addr)));
            length_ = 2;
            break;
        case 0x0C: // aaaa
            data_addr = ram->ReadWord(addr);
            operands_.push_back((Operand(Operand::kAbsolute, data_addr)));
            length_ = 3;
            break;
        case 0x10: // (aa), Y
            data_addr = ram->ReadByte(addr);
            operands_.push_back((Operand(Operand::kIndirectIndexed, data_addr)));
            length_ = 2;
            break;
        case 0x14: // aa, X
            data_addr = ram->ReadByte(addr);
            operands_.push_back(Operand(Operand::kAbsoluteIndexedX, data_addr));
            length_ = 2;
            break;
        case 0x18: // aaaa, Y
            data_addr = ram->ReadWord(addr);
            operands_.push_back(Operand(Operand::kAbsoluteIndexedY));
            length_ = 3;
            break;
        case 0x1C: // aaaa, X
            data_addr = ram->ReadWord(addr);
            operands_.push_back(Operand(Operand::kAbsoluteIndexedY));
            length_ = 3;
            break;
        default:
            std::stringstream ss;
            ss << "Unknown opcode: 0x" << std::hex << opcode;
            throw std::runtime_error(ss.str());
        }
    }

    size_t Instruction::GetLength() {
        return length_;
    }

    Operand Instruction::GetOperand(size_t n) {
        return operands_[n];
    }

    Instruction::Mnemonic Instruction::GetMnemonic() {
        return mnemonic_;
    }
}
