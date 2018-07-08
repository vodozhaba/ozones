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
        uint8_t opcode = ram->ReadByte(addr++);
        if(opcode & 0x03 == 0x01) {
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
            uint16_t data_addr;
            switch(opcode & 0x0C) {
            case 0x00: // (aa, X)
                data_addr = ram->ReadByte(addr);
                operand_ = Operand(Operand::kIndexedIndirect, data_addr);
                length_ = 2;
                break;
            case 0x04: // aa
                data_addr = ram->ReadByte(addr);
                operand_ = (Operand(Operand::kAbsolute, data_addr));
                length_ = 2;
                break;
            case 0x08: // #aa
                data_addr = ram->ReadByte(addr); // Not really an address but an immediate value
                operand_ = (Operand(Operand::kImmediate, data_addr));
                length_ = 2;
                break;
            case 0x0C: // aaaa
                data_addr = ram->ReadWord(addr);
                operand_ = (Operand(Operand::kAbsolute, data_addr));
                length_ = 3;
                break;
            case 0x10: // (aa), Y
                data_addr = ram->ReadByte(addr);
                operand_ = (Operand(Operand::kIndirectIndexed, data_addr));
                length_ = 2;
                break;
            case 0x14: // aa, X
                data_addr = ram->ReadByte(addr);
                operand_ = Operand(Operand::kAbsoluteIndexedX, data_addr);
                length_ = 2;
                break;
            case 0x18: // aaaa, Y
                data_addr = ram->ReadWord(addr);
                operand_ = Operand(Operand::kAbsoluteIndexedY, data_addr);
                length_ = 3;
                break;
            case 0x1C: // aaaa, X
                data_addr = ram->ReadWord(addr);
                operand_ = Operand(Operand::kAbsoluteIndexedY, data_addr);
                length_ = 3;
                break;
            default:
                std::stringstream ss;
                ss << "Unknown opcode: 0x" << std::hex << opcode;
                throw std::runtime_error(ss.str());
            }
            return;
        }
        switch(opcode) {
        case 0x00:
            mnemonic_ = kBrk;
            break;
        case 0x02:
        case 0x12:
        case 0x22:
        case 0x32:
        case 0x42:
        case 0x52:
        case 0x62:
        case 0x72:
        case 0x92:
        case 0xB2:
        case 0xD2:
        case 0xF2:
            mnemonic_ = kStp;
            length_ = 1;
            break;
        case 0x04:
        case 0x14:
        case 0x34: // sic, 0x24 is BIT aa
        case 0x44:
        case 0x54:
        case 0x64:
        case 0x74:
        case 0x80:
        case 0x82:
        case 0xC2:
        case 0xC4:
        case 0xE2:
        case 0xE4:
            mnemonic_ = kNop;
            length_ = 2;
            break;
        case 0x06:
            mnemonic_ = kAsl;
            operand_ = Operand(Operand::kAbsolute, ram->ReadByte(addr));
            length_ = 2;
            break;
        case 0x08:
            mnemonic_ = kPhp;
            length_ = 1;
            break;
        case 0x0A:
            mnemonic_ = kAsl;
            length_ = 1;
            break;
        case 0x0C:
        case 0x1C:
        case 0x3C:
        case 0x5C:
        case 0x7C:
        case 0xDC:
        case 0xEC:
            mnemonic_ = kNop;
            length_ = 3;
            break;
        case 0x0E:
            mnemonic_ = kAsl;
            operand_ = Operand(Operand::kAbsolute, ram->ReadWord(addr));
            length_ = 3;
            break;
        case 0x10:
            mnemonic_ = kBpl;
            operand_ = Operand(Operand::kRelative, ram->ReadByte(addr));
            length_ = 2;
            break;
        case 0x16:
            mnemonic_ = kAsl;
            operand_ = Operand(Operand::kAbsoluteIndexedX, ram->ReadByte(addr));
            length_ = 2;
            break;
        case 0x18:
            mnemonic_ = kClc;
            length_ = 1;
            break;
        case 0x1A:
        case 0x3A:
        case 0x5A:
        case 0x7A:
        case 0xCA:
        case 0xEA:
            mnemonic_ = kNop;
            length_ = 1;
            break;
        case 0x1E:
            mnemonic_ = kAsl;
            operand_ = Operand(Operand::kAbsoluteIndexedX, ram->ReadWord(addr));
            length_ = 3;
            break;
        case 0x20:
            mnemonic_ = kJsr;
            operand_ = Operand(Operand::kAbsolute, addr);
            length_ = 3;
            break;
        case 0x24:
            mnemonic_ = kBit;
            operand_ = Operand(Operand::kAbsolute, ram->ReadByte(addr));
            length_ = 2;
            break;
        case 0x26:
            mnemonic_ = kRol;
            operand_ = Operand(Operand::kAbsolute, ram->ReadByte(addr));
            length_ = 2;
            break;
        case 0x28:
            mnemonic_ = kPlp;
            length_ = 1;
            break;
        case 0x2A:
            mnemonic_ = kRol;
            length_ = 1;
            break;
        case 0x2C:
            mnemonic_ = kBit;
            operand_ = Operand(Operand::kAbsolute, ram->ReadWord(addr));
            length_ = 3;
            break;
        case 0x2E:
            mnemonic_ = kRol;
            operand_ = Operand(Operand::kAbsolute, ram->ReadWord(addr));
            length_ = 3;
            break;
        case 0x30:
            mnemonic_ = kBmi;
            operand_ = Operand(Operand::kRelative, ram->ReadByte(addr));
            length_ = 2;
            break;
        case 0x36:
            mnemonic_ = kRol;
            operand_ = Operand(Operand::kAbsoluteIndexedX, ram->ReadByte(addr));
            length_ = 2;
            break;
        case 0x38:
            mnemonic_ = kSec;
            break;
        case 0x3E:
            mnemonic_ = kRol;
            operand_ = Operand(Operand::kAbsoluteIndexedX, ram->ReadWord(addr));
            length_ = 2;
            break;
        case 0x40:
            mnemonic_ = kRti;
            length_ = 1;
            break;
        case 0x46:
            mnemonic_ = kLsr;
            operand_ = Operand(Operand::kAbsolute, ram->ReadByte(addr));
            length_ = 2;
            break;
        case 0x48:
            mnemonic_ = kPha;
            length_ = 1;
            break;
        case 0x4A:
            mnemonic_ = kLsr;
            length_ = 1;
            break;
        case 0x4C:
            mnemonic_ = kJmp;
            operand_ = Operand(Operand::kAbsolute, addr);
            length_ = 3;
            break;
        case 0x4E:
            mnemonic_ = kLsr;
            operand_ = Operand(Operand::kAbsolute, ram->ReadWord(addr));
            length_ = 3;
            break;
        case 0x50:
            mnemonic_ = kBvc;
            operand_ = Operand(Operand::kRelative, ram->ReadByte(addr));
            length_ = 2;
            break;
        case 0x56:
            mnemonic_ = kRor;
            operand_ = Operand(Operand::kAbsoluteIndexedX, ram->ReadByte(addr));
            length_ = 2;
            break;
        case 0x58:
            mnemonic_ = kClc;
            length_ = 1;
            break;
        case 0x5E:
            mnemonic_ = kLsr;
            operand_ = Operand(Operand::kAbsoluteIndexedX, ram->ReadWord(addr));
            length_ = 3;
            break;
        case 0x60:
            mnemonic_ = kRts;
            length_ = 1;
            break;
        case 0x66:
            mnemonic_ = kRor;
            operand_ = Operand(Operand::kAbsolute, addr);
            length_ = 2;
            break;
        case 0x68:
            mnemonic_ = kPla;
            length_ = 1;
            break;
        case 0x6A:
            mnemonic_ = kRor;
            length_ = 1;
            break;
        case 0x6C:
            mnemonic_ = kJmp;
            operand_ = Operand(Operand::kAbsolute, ram->ReadWord(addr));
            length_ = 3;
            break;
        case 0x6E:
            mnemonic_ = kRor;
            operand = Operand(Operand::kAbsolute, ram->ReadWord(addr));
            length_ = 3;
            break;
        case 0x70:
            mnemonic_ = kBvs;
            operand_ = Operand(Operand::kRelative, ram->ReadByte(addr));
            length_ = 2;
            break;
        case 0x76:
            mnemonic_ = kRor;
            operand_ = Operand(Operand::kAbsoluteIndexedX, ram->ReadByte(addr));
            length_ = 2;
            break;
        case 0x78:
            mnemonic_ = kSei;
            length_ = 1;
            break;
        case 0x7E:
            mnemonic_ = kRor;
            operand_ = Operand(Operand::kAbsoluteIndexedX, ram->ReadWord(addr));
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

    Operand Instruction::GetOperand() {
        return operand_;
    }

    Instruction::Mnemonic Instruction::GetMnemonic() {
        return mnemonic_;
    }
}
