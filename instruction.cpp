// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "instruction.h"
#include <sstream>

namespace ozones {

    Operand::Operand(Operand::AddressingMode mode, uint16_t addr, bool page_boundary_penalty) : mode_(mode), page_boundary_penalty_(page_boundary_penalty), value_(addr) { }

    Operand::AddressingMode Operand::GetMode() {
        return mode_;
    }

    bool Operand::HasPageBoundaryPenalty() {
        return page_boundary_penalty_;
    }

    uint16_t Operand::GetValue() {
        return value_;
    }

    Instruction::Instruction(std::shared_ptr<Ram> ram, uint16_t addr) : cycles_(2), length_(1) {
        uint8_t opcode = ram->ReadByte(addr++);
        switch(opcode) {
        // Official opcodes
        case 0x00:
            mnemonic_ = kBrk;
            cycles_ = 7;
            break;
        case 0x01:
            mnemonic_ = kOra;
            operand_ = Operand(Operand::kIndexedIndirect, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 6;
            break;
        case 0x05:
            mnemonic_ = kOra;
            operand_ = Operand(Operand::kAbsolute, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 3;
            break;
        case 0x06:
            mnemonic_ = kAsl;
            operand_ = Operand(Operand::kAbsolute, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 5;
            break;
        case 0x08:
            mnemonic_ = kPhp;
            cycles_ = 3;
            break;
        case 0x09:
            mnemonic_ = kOra;
            operand_ = Operand(Operand::kImmediate, ram->ReadByte(addr));
            length_ = 2;
            break;
        case 0x0A:
            mnemonic_ = kAsl;
            operand_ = Operand(Operand::kAccumulator);
            break;
        case 0x0D:
            mnemonic_ = kOra;
            operand_ = Operand(Operand::kAbsolute, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 4;
            break;
        case 0x0E:
            mnemonic_ = kAsl;
            operand_ = Operand(Operand::kAbsolute, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 6;
            break;
        case 0x10:
            mnemonic_ = kBpl;
            operand_ = Operand(Operand::kRelative, ram->ReadByte(addr), true);
            length_ = 2;
            break;
        case 0x11:
            mnemonic_ = kOra;
            operand_ = Operand(Operand::kIndirectIndexed, ram->ReadByte(addr), true);
            length_ = 2;
            cycles_ = 5;
            break;
        case 0x15:
            mnemonic_ = kOra;
            operand_ = Operand(Operand::kZeroPageIndexedX, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 4;
            break;
        case 0x16:
            mnemonic_ = kAsl;
            operand_ = Operand(Operand::kZeroPageIndexedX, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 6;
            break;
        case 0x18:
            mnemonic_ = kClc;
            break;
        case 0x19:
            mnemonic_ = kOra;
            operand_ = Operand(Operand::kAbsoluteIndexedY, ram->ReadWord(addr), true);
            length_ = 3;
            cycles_ = 4;
            break;
        case 0x1D:
            mnemonic_ = kOra;
            operand_ = Operand(Operand::kAbsoluteIndexedX, ram->ReadWord(addr), true);
            length_ = 3;
            cycles_ = 4;
            break;
        case 0x1E:
            mnemonic_ = kAsl;
            operand_ = Operand(Operand::kAbsoluteIndexedX, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 7;
            break;
        case 0x20:
            mnemonic_ = kJsr;
            operand_ = Operand(Operand::kImmediate, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 6;
            break;
        case 0x21:
            mnemonic_ = kAnd;
            operand_ = Operand(Operand::kIndexedIndirect, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 6;
            break;
        case 0x24:
            mnemonic_ = kBit;
            operand_ = Operand(Operand::kAbsolute, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 3;
            break;
        case 0x25:
            mnemonic_ = kAnd;
            operand_ = Operand(Operand::kAbsolute, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 3;
            break;
        case 0x26:
            mnemonic_ = kRol;
            operand_ = Operand(Operand::kAbsolute, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 5;
            break;
        case 0x28:
            mnemonic_ = kPlp;
            cycles_ = 4;
            break;
        case 0x29:
            mnemonic_ = kAnd;
            operand_ = Operand(Operand::kImmediate, ram->ReadByte(addr));
            length_ = 2;
            break;
        case 0x2A:
            mnemonic_ = kRol;
            operand_ = Operand(Operand::kAccumulator);
            break;
        case 0x2C:
            mnemonic_ = kBit;
            operand_ = Operand(Operand::kAbsolute, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 4;
            break;
        case 0x2D:
            mnemonic_ = kAnd;
            operand_ = Operand(Operand::kAbsolute, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 4;
            break;
        case 0x2E:
            mnemonic_ = kRol;
            operand_ = Operand(Operand::kAbsolute, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 6;
            break;
        case 0x30:
            mnemonic_ = kBmi;
            operand_ = Operand(Operand::kRelative, ram->ReadByte(addr), true);
            length_ = 2;
            break;
        case 0x31:
            mnemonic_ = kAnd;
            operand_ = Operand(Operand::kIndirectIndexed, ram->ReadByte(addr), true);
            length_ = 2;
            cycles_ = 5;
            break;
        case 0x35:
            mnemonic_ = kAnd;
            operand_ = Operand(Operand::kZeroPageIndexedX, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 4;
            break;
        case 0x36:
            mnemonic_ = kRol;
            operand_ = Operand(Operand::kZeroPageIndexedX, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 6;
            break;
        case 0x38:
            mnemonic_ = kSec;
            break;
        case 0x39:
            mnemonic_ = kAnd;
            operand_ = Operand(Operand::kAbsoluteIndexedY, ram->ReadWord(addr), true);
            length_ = 3;
            cycles_ = 4;
            break;
        case 0x3D:
            mnemonic_ = kAnd;
            operand_ = Operand(Operand::kAbsoluteIndexedX, ram->ReadWord(addr), true);
            length_ = 3;
            cycles_ = 4;
            break;
        case 0x3E:
            mnemonic_ = kRol;
            operand_ = Operand(Operand::kAbsoluteIndexedX, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 7;
            break;
        case 0x40:
            mnemonic_ = kRti;
            cycles_ = 6;
            break;
        case 0x41:
            mnemonic_ = kEor;
            operand_ = Operand(Operand::kIndexedIndirect, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 6;
            break;
        case 0x45:
            mnemonic_ = kEor;
            operand_ = Operand(Operand::kAbsolute, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 3;
            break;
        case 0x46:
            mnemonic_ = kLsr;
            operand_ = Operand(Operand::kAbsolute, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 5;
            break;
        case 0x48:
            mnemonic_ = kPha;
            cycles_ = 3;
            break;
        case 0x49:
            mnemonic_ = kEor;
            operand_ = Operand(Operand::kImmediate, ram->ReadByte(addr));
            length_ = 2;
            break;
        case 0x4A:
            mnemonic_ = kLsr;
            operand_ = Operand(Operand::kAccumulator);
            break;
        case 0x4C:
            mnemonic_ = kJmp;
            operand_ = Operand(Operand::kImmediate, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 3;
            break;
        case 0x4D:
            mnemonic_ = kEor;
            operand_ = Operand(Operand::kAbsolute, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 4;
            break;
        case 0x4E:
            mnemonic_ = kLsr;
            operand_ = Operand(Operand::kAbsolute, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 6;
            break;
        case 0x50:
            mnemonic_ = kBvc;
            operand_ = Operand(Operand::kRelative, ram->ReadByte(addr), true);
            length_ = 2;
            break;
        case 0x51:
            mnemonic_ = kEor;
            operand_ = Operand(Operand::kIndirectIndexed, ram->ReadByte(addr), true);
            length_ = 2;
            cycles_ = 5;
            break;
        case 0x55:
            mnemonic_ = kEor;
            operand_ = Operand(Operand::kZeroPageIndexedX, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 4;
            break;
        case 0x56:
            mnemonic_ = kLsr;
            operand_ = Operand(Operand::kZeroPageIndexedX, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 6;
            break;
        case 0x58:
            mnemonic_ = kClc;
            break;
        case 0x59:
            mnemonic_ = kEor;
            operand_ = Operand(Operand::kAbsoluteIndexedY, ram->ReadWord(addr), true);
            length_ = 3;
            cycles_ = 4;
            break;
        case 0x5D:
            mnemonic_ = kEor;
            operand_ = Operand(Operand::kAbsoluteIndexedX, ram->ReadWord(addr), true);
            length_ = 3;
            cycles_ = 4;
            break;
        case 0x5E:
            mnemonic_ = kLsr;
            operand_ = Operand(Operand::kAbsoluteIndexedX, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 7;
            break;
        case 0x60:
            mnemonic_ = kRts;
            cycles_ = 6;
            break;
        case 0x61:
            mnemonic_ = kAdc;
            operand_ = Operand(Operand::kIndexedIndirect, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 6;
            break;
        case 0x65:
            mnemonic_ = kAdc;
            operand_ = Operand(Operand::kAbsolute, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 3;
            break;
        case 0x66:
            mnemonic_ = kRor;
            operand_ = Operand(Operand::kAbsolute, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 5;
            break;
        case 0x68:
            mnemonic_ = kPla;
            cycles_ = 4;
            break;
        case 0x69:
            mnemonic_ = kAdc;
            operand_ = Operand(Operand::kImmediate, ram->ReadByte(addr));
            length_ = 2;
            break;
        case 0x6A:
            mnemonic_ = kRor;
            operand_ = Operand(Operand::kAccumulator);
            break;
        case 0x6C: {
            mnemonic_ = kJmp;
            uint16_t op_addr = ram->ReadWord(addr);
            uint16_t result;
            // emulate the indirect JMP hardware bug
            if((op_addr & 0xFF) == 0xFF) {
                result = ram->ReadByte(op_addr) & 0xFF;
                result |= ram->ReadByte(op_addr & 0xFF00) << 8;
            } else {
                result = ram->ReadWord(op_addr);
            }
            operand_ = Operand(Operand::kImmediate, result);
            length_ = 3;
            cycles_ = 5;
            break;
        }
        case 0x6D:
            mnemonic_ = kAdc;
            operand_ = Operand(Operand::kAbsolute, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 4;
            break;
        case 0x6E:
            mnemonic_ = kRor;
            operand_ = Operand(Operand::kAbsolute, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 6;
            break;
        case 0x70:
            mnemonic_ = kBvs;
            operand_ = Operand(Operand::kRelative, ram->ReadByte(addr), true);
            length_ = 2;
            break;
        case 0x71:
            mnemonic_ = kAdc;
            operand_ = Operand(Operand::kIndirectIndexed, ram->ReadByte(addr), true);
            length_ = 2;
            cycles_ = 5;
            break;
        case 0x75:
            mnemonic_ = kAdc;
            operand_ = Operand(Operand::kZeroPageIndexedX, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 4;
            break;
        case 0x76:
            mnemonic_ = kRor;
            operand_ = Operand(Operand::kZeroPageIndexedX, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 6;
            break;
        case 0x78:
            mnemonic_ = kSei;
            break;
        case 0x79:
            mnemonic_ = kAdc;
            operand_ = Operand(Operand::kAbsoluteIndexedY, ram->ReadWord(addr), true);
            length_ = 3;
            cycles_ = 4;
            break;
        case 0x7D:
            mnemonic_ = kAdc;
            operand_ = Operand(Operand::kAbsoluteIndexedX, ram->ReadWord(addr), true);
            length_ = 3;
            cycles_ = 4;
            break;
        case 0x7E:
            mnemonic_ = kRor;
            operand_ = Operand(Operand::kAbsoluteIndexedX, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 7;
            break;
        case 0x81:
            mnemonic_ = kSta;
            operand_ = Operand(Operand::kIndexedIndirect, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 6;
            break;
        case 0x84:
            mnemonic_ = kSty;
            operand_ = Operand(Operand::kAbsolute, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 3;
            break;
        case 0x85:
            mnemonic_ = kSta;
            operand_ = Operand(Operand::kAbsolute, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 3;
            break;
        case 0x86:
            mnemonic_ = kStx;
            operand_ = Operand(Operand::kAbsolute, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 3;
            break;
        case 0x88:
            mnemonic_ = kDey;
            break;
        case 0x8A:
            mnemonic_ = kTxa;
            break;
        case 0x8C:
            mnemonic_ = kSty;
            operand_ = Operand(Operand::kAbsolute, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 4;
            break;
        case 0x8D:
            mnemonic_ = kSta;
            operand_ = Operand(Operand::kAbsolute, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 4;
            break;
        case 0x8E:
            mnemonic_ = kStx;
            operand_ = Operand(Operand::kAbsolute, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 4;
            break;
        case 0x90:
            mnemonic_ = kBcc;
            operand_ = Operand(Operand::kRelative, ram->ReadByte(addr), true);
            length_ = 2;
            break;
        case 0x91:
            mnemonic_ = kSta;
            operand_ = Operand(Operand::kIndirectIndexed, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 6;
            break;
        case 0x94:
            mnemonic_ = kSty;
            operand_ = Operand(Operand::kZeroPageIndexedX, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 4;
            break;
        case 0x95:
            mnemonic_ = kSta;
            operand_ = Operand(Operand::kZeroPageIndexedX, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 4;
            break;
        case 0x96:
            mnemonic_ = kStx;
            operand_ = Operand(Operand::kZeroPageIndexedY, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 4;
            break;
        case 0x98:
            mnemonic_ = kTya;
            break;
        case 0x99:
            mnemonic_ = kSta;
            operand_ = Operand(Operand::kAbsoluteIndexedY, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 5;
            break;
        case 0x9A:
            mnemonic_ = kTxs;
            break;
        case 0x9D:
            mnemonic_ = kSta;
            operand_ = Operand(Operand::kAbsoluteIndexedX, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 5;
            break;
        case 0xA0:
            mnemonic_ = kLdy;
            operand_ = Operand(Operand::kImmediate, ram->ReadByte(addr));
            length_ = 2;
            break;
        case 0xA1:
            mnemonic_ = kLda;
            operand_ = Operand(Operand::kIndexedIndirect, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 6;
            break;
        case 0xA2:
            mnemonic_ = kLdx;
            operand_ = Operand(Operand::kImmediate, ram->ReadByte(addr));
            length_ = 2;
            break;
        case 0xA4:
            mnemonic_ = kLdy;
            operand_ = Operand(Operand::kAbsolute, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 3;
            break;
        case 0xA5:
            mnemonic_ = kLda;
            operand_ = Operand(Operand::kAbsolute, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 3;
            break;
        case 0xA6:
            mnemonic_ = kLdx;
            operand_ = Operand(Operand::kAbsolute, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 3;
            break;
        case 0xA8:
            mnemonic_ = kTay;
            break;
        case 0xA9:
            mnemonic_ = kLda;
            operand_ = Operand(Operand::kImmediate, ram->ReadByte(addr));
            length_ = 2;
            break;
        case 0xAA:
            mnemonic_ = kTax;
            break;
        case 0xAC:
            mnemonic_ = kLdy;
            operand_ = Operand(Operand::kAbsolute, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 4;
            break;
        case 0xAD:
            mnemonic_ = kLda;
            operand_ = Operand(Operand::kAbsolute, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 4;
            break;
        case 0xAE:
            mnemonic_ = kLdx;
            operand_ = Operand(Operand::kAbsolute, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 4;
            break;
        case 0xB0:
            mnemonic_ = kBcs;
            operand_ = Operand(Operand::kRelative, ram->ReadByte(addr), true);
            length_ = 2;
            break;
        case 0xB1:
            mnemonic_ = kLda;
            operand_ = Operand(Operand::kIndirectIndexed, ram->ReadByte(addr), true);
            length_ = 2;
            cycles_ = 5;
            break;
        case 0xB4:
            mnemonic_ = kLdy;
            operand_ = Operand(Operand::kZeroPageIndexedX, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 4;
            break;
        case 0xB5:
            mnemonic_ = kLda;
            operand_ = Operand(Operand::kZeroPageIndexedX, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 4;
            break;
        case 0xB6:
            mnemonic_ = kLdx;
            operand_ = Operand(Operand::kZeroPageIndexedY, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 4;
            break;
        case 0xB8:
            mnemonic_ = kClv;
            break;
        case 0xB9:
            mnemonic_ = kLda;
            operand_ = Operand(Operand::kAbsoluteIndexedY, ram->ReadWord(addr), true);
            length_ = 3;
            cycles_ = 4;
            break;
        case 0xBA:
            mnemonic_ = kTsx;
            break;
        case 0xBC:
            mnemonic_ = kLdy;
            operand_ = Operand(Operand::kAbsoluteIndexedX, ram->ReadWord(addr), true);
            length_ = 3;
            cycles_ = 4;
            break;
        case 0xBD:
            mnemonic_ = kLda;
            operand_ = Operand(Operand::kAbsoluteIndexedX, ram->ReadWord(addr), true);
            length_ = 3;
            cycles_ = 4;
            break;
        case 0xBE:
            mnemonic_ = kLdx;
            operand_ = Operand(Operand::kAbsoluteIndexedY, ram->ReadWord(addr), true);
            length_ = 3;
            cycles_ = 4;
            break;
        case 0xC0:
            mnemonic_ = kCpy;
            operand_ = Operand(Operand::kImmediate, ram->ReadByte(addr));
            length_ = 2;
            break;
        case 0xC1:
            mnemonic_ = kCmp;
            operand_ = Operand(Operand::kIndexedIndirect, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 6;
            break;
        case 0xC4:
            mnemonic_ = kCpy;
            operand_ = Operand(Operand::kAbsolute, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 3;
            break;
        case 0xC5:
            mnemonic_ = kCmp;
            operand_ = Operand(Operand::kAbsolute, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 3;
            break;
        case 0xC6:
            mnemonic_ = kDec;
            operand_ = Operand(Operand::kAbsolute, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 5;
            break;
        case 0xC8:
            mnemonic_ = kIny;
            break;
        case 0xC9:
            mnemonic_ = kCmp;
            operand_ = Operand(Operand::kImmediate, ram->ReadByte(addr));
            length_ = 2;
            break;
        case 0xCA:
            mnemonic_ = kDex;
            break;
        case 0xCC:
            mnemonic_ = kCpy;
            operand_ = Operand(Operand::kAbsolute, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 4;
            break;
        case 0xCD:
            mnemonic_ = kCmp;
            operand_ = Operand(Operand::kAbsolute, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 4;
            break;
        case 0xCE:
            mnemonic_ = kDec;
            operand_ = Operand(Operand::kAbsolute, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 6;
            break;
        case 0xD0:
            mnemonic_ = kBne;
            operand_ = Operand(Operand::kRelative, ram->ReadByte(addr), true);
            length_ = 2;
            break;
        case 0xD1:
            mnemonic_ = kCmp;
            operand_ = Operand(Operand::kIndirectIndexed, ram->ReadByte(addr), true);
            length_ = 2;
            cycles_ = 5;
            break;
        case 0xD5:
            mnemonic_ = kCmp;
            operand_ = Operand(Operand::kZeroPageIndexedX, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 4;
            break;
        case 0xD6:
            mnemonic_ = kDec;
            operand_ = Operand(Operand::kZeroPageIndexedX, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 6;
            break;
        case 0xD8:
            mnemonic_ = kCld;
            break;
        case 0xD9:
            mnemonic_ = kCmp;
            operand_ = Operand(Operand::kAbsoluteIndexedY, ram->ReadWord(addr), true);
            length_ = 3;
            cycles_ = 4;
            break;
        case 0xDD:
            mnemonic_ = kCmp;
            operand_ = Operand(Operand::kAbsoluteIndexedX, ram->ReadWord(addr), true);;
            length_ = 3;
            cycles_ = 4;
            break;
        case 0xDE:
            mnemonic_ = kDec;
            operand_ = Operand(Operand::kAbsoluteIndexedX, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 7;
            break;
        case 0xE0:
            mnemonic_ = kCpx;
            operand_ = Operand(Operand::kImmediate, ram->ReadByte(addr));
            length_ = 2;
            break;
        case 0xE1:
            mnemonic_ = kSbc;
            operand_ = Operand(Operand::kIndexedIndirect, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 6;
            break;
        case 0xE4:
            mnemonic_ = kCpx;
            operand_ = Operand(Operand::kAbsolute, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 3;
            break;
        case 0xE5:
            mnemonic_ = kSbc;
            operand_ = Operand(Operand::kAbsolute, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 3;
            break;
        case 0xE6:
            mnemonic_ = kInc;
            operand_ = Operand(Operand::kAbsolute, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 5;
            break;
        case 0xE8:
            mnemonic_ = kInx;
            break;
        case 0xE9:
            mnemonic_ = kSbc;
            operand_ = Operand(Operand::kImmediate, ram->ReadByte(addr));
            length_ = 2;
            break;
        case 0xEA:
            mnemonic_ = kNop;
            break;
        case 0xEC:
            mnemonic_ = kCpx;
            operand_ = Operand(Operand::kAbsolute, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 4;
            break;
        case 0xED:
            mnemonic_ = kSbc;
            operand_ = Operand(Operand::kAbsolute, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 4;
            break;
        case 0xEE:
            mnemonic_ = kInc;
            operand_ = Operand(Operand::kAbsolute, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 6;
            break;
        case 0xF0:
            mnemonic_ = kBeq;
            operand_ = Operand(Operand::kRelative, ram->ReadByte(addr), true);
            length_ = 2;
            break;
        case 0xF1:
            mnemonic_ = kSbc;
            operand_ = Operand(Operand::kIndirectIndexed, ram->ReadByte(addr), true);
            length_ = 2;
            cycles_ = 5;
            break;
        case 0xF5:
            mnemonic_ = kSbc;
            operand_ = Operand(Operand::kZeroPageIndexedX, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 4;
            break;
        case 0xF6:
            mnemonic_ = kInc;
            operand_ = Operand(Operand::kZeroPageIndexedX, ram->ReadByte(addr));
            length_ = 2;
            cycles_ = 6;
            break;
        case 0xF8:
            mnemonic_ = kSed;
            break;
        case 0xF9:
            mnemonic_ = kSbc;
            operand_ = Operand(Operand::kAbsoluteIndexedY, ram->ReadWord(addr), true);
            length_ = 3;
            cycles_ = 4;
            break;
        case 0xFD:
            mnemonic_ = kSbc;
            operand_ = Operand(Operand::kAbsoluteIndexedX, ram->ReadWord(addr), true);
            length_ = 3;
            cycles_ = 4;
            break;
        case 0xFE:
            mnemonic_ = kInc;
            operand_ = Operand(Operand::kAbsoluteIndexedX, ram->ReadWord(addr));
            length_ = 3;
            cycles_ = 7;
            break;
        // Unofficial opcodes
        case 0x04:
        case 0x44:
        case 0x64:
            mnemonic_ = kNop;
            length_ = 2;
            cycles_ = 3;
            break;
        default:
            std::stringstream ss;
            ss << std::hex << "Unknown opcode: 0x" << (int) opcode;
            throw std::runtime_error(ss.str());
        }
    }

    int Instruction::GetCycles() {
        return cycles_;
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
