#pragma once

#include <cstdarg>
#include <cstdint>
#include <memory>
#include <vector>
#include "ram.h"

namespace ozones {

    class Operand {
    public:
        // Zero page addresses are converted to absolute during decoding
        enum AddressingMode {
            kImmediate,
            kAbsolute,
            kImplied,
            kIndirectAbsolute,
            kAbsoluteIndexedX,
            kAbsoluteIndexedY,
            kIndexedIndirect,
            kIndirectIndexed,
            kRelative,
            kAccumulator
        };
        Operand(AddressingMode mode, uint16_t addr = 0);
        AddressingMode GetMode();
        uint16_t GetValue();
    private:
        AddressingMode mode_;
        uint16_t value_;
    };

    class Instruction {
    public:
        enum Mnemonic {
            kStp,
            kNop,
            kBrk,
            kPhp,
            kBpl,
            kClc,
            kJsr,
            kBit,
            kPlp,
            kBmi,
            kSec,
            kRti,
            kPha,
            kJmp,
            kBvc,
            kCli,
            kRts,
            kPla,
            kBvs,
            kSei,
            kSty,
            kDey,
            kBcc,
            kTya,
            kLdy,
            kTay,
            kBcs,
            kClv,
            kCpy,
            kIny,
            kBne,
            kCld,
            kCpx,
            kInx,
            kBeq,
            kSed,
            kOra,
            kAnd,
            kEor,
            kAdc,
            kSta,
            kLda,
            kCmp,
            kSbc,
            kAsl,
            kRol,
            kLsr,
            kRor,
            kStx,
            kTxa,
            kTxs,
            kShx,
            kLdx,
            kTax,
            kTsx,
            kDec,
            kDex,
            kInc
        };
        Instruction(std::shared_ptr<Ram> ram, uint16_t addr);
        size_t GetLength();
        Operand GetOperand(size_t n);
        Mnemonic GetMnemonic();
    private:
        size_t length_;
        std::vector<Operand> operands_;
        Mnemonic mnemonic_;
    };

}
