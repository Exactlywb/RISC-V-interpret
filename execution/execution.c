#include "execution.h"

//===============LOGGING===============

#define SHOW_REG_BEFORE(command)                                                                                        \
                                    do {                                                                                \
                                                                                                                        \
                                        printf ("\n\n");                                                                \
                                        printf ("* \t %s:\n", command);                                                 \
                                        printf ("** \t before: reg [%d] = %u\n", numRd, CPU->registers [numRd]);        \
                                                                                                                        \
                                    } while (0)

#define SHOW_REG_AFTER_TWO_REGS(sign)                                                                                       \
                                    do {                                                                                    \
                                                                                                                            \
                                        printf ("--------------------------------------\n");                                \
                                        printf ("** \t after: reg [%d] = %u %s %u = %u\n",  numRd, CPU->registers [numRs1], \
                                                                                            sign, CPU->registers [numRs2],  \
                                                                                            RD);                            \
                                        printf ("--------------------------------------\n");                                \
                                                                                                                            \
                                    } while (0)


#define SHOW_REG_AFTER_IMM_REG(sign)                                                                                            \
                                    do {                                                                                        \
                                                                                                                                \
                                        printf ("--------------------------------------\n");                                    \
                                        printf ("** \t after: reg [%d] = %u %s %u = %u\n",  numRd, CPU->registers [numRs1],     \
                                                                                            sign, imm, RD);                     \
                                        printf ("--------------------------------------\n");                                    \
                                                                                                                                \
                                    } while (0)

#define CHECK_CPU   SECURITY_FUNCTION (IS_NULL (CPU), {}, -1)
#define MOVE_PC     PC += 4

#define IMPL(toDo)      do {                        \
                                                    \
                            CHECK_CPU;              \
                            toDo                    \
                            MOVE_PC;                \
                            return 0;               \
                                                    \
                        } while (0)

#define RS1 (CPU->registers [numRs1])
#define RS2 (CPU->registers [numRs2])

#define RD  (CPU->registers[numRd])

#define PC  (CPU->pc)

#define CHECK_RD_X0     if (numRd == 0) return 0

#define MEMORY CPU->memory

//=========================================================================
//================================ARITHMETIC===============================
//=========================================================================
char ImplAdd  (CPU* CPU, const RegNumber numRs1, const RegNumber numRs2, const RegNumber numRd) {

    IMPL 
    (
        {
            SHOW_REG_BEFORE ("add");
            CHECK_RD_X0;
            RD = RS1 + RS2;
            SHOW_REG_AFTER_TWO_REGS ("+");
        }
    );

}

char ImplAddI (CPU* CPU, const ImmValue imm, const RegNumber numRs1, const RegNumber numRd) {

    IMPL 
    (
        {
            SHOW_REG_BEFORE ("addI");
            CHECK_RD_X0;
            RD = (RegValue)((ImmValue)RS1 + imm);
            SHOW_REG_AFTER_IMM_REG ("+");
        }
    );

}

char ImplSub  (CPU* CPU, const RegNumber numRs1, const RegNumber numRs2, const RegNumber numRd) {

    IMPL 
    (
        {
            SHOW_REG_BEFORE ("sub");
            CHECK_RD_X0;
            RD = RS1 - RS2;
            SHOW_REG_AFTER_TWO_REGS ("-");
        }
    );

}

//=========================================================================
//==============================LOGICAL OPERS==============================
//=========================================================================
char ImplAnd  (CPU* CPU, const RegNumber numRs1, const RegNumber numRs2, const RegNumber numRd) {

    IMPL 
    (
        {
            SHOW_REG_BEFORE ("sub");
            CHECK_RD_X0;
            RD = RS1 & RS2;
            SHOW_REG_AFTER_TWO_REGS ("-");
        }
    );

}

char ImplAndI  (CPU* CPU, const ImmValue  imm, const RegNumber numRs1, const RegNumber numRd) {
    
    IMPL 
    (
        {
            SHOW_REG_BEFORE ("andI");
            CHECK_RD_X0;
            RD = (RegValue)((ImmValue)RS1 & imm);
            SHOW_REG_AFTER_IMM_REG ("&");
        }
    );

}

char ImplOr    (CPU* CPU, const RegNumber numRs1, const RegNumber numRs2, const RegNumber numRd) {

    IMPL 
    (
        {
            SHOW_REG_BEFORE ("or");
            CHECK_RD_X0;
            RD = RS1 | RS2;
            SHOW_REG_AFTER_TWO_REGS ("|");
        }
    );

}

char ImplOrI   (CPU* CPU, const ImmValue imm, const RegNumber numRs1, const RegNumber numRd) {
    
    IMPL 
    (
        {
            SHOW_REG_BEFORE ("orI");
            CHECK_RD_X0;
            RD = (RegValue)((ImmValue)RS1 | imm);
            SHOW_REG_AFTER_IMM_REG ("|");
        }
    );

}

char ImplXor   (CPU* CPU, const RegNumber numRs1, const RegNumber numRs2, const RegNumber numRd) {

    IMPL 
    (
        {
            SHOW_REG_BEFORE ("xor");
            CHECK_RD_X0;
            RD = RS1 ^ RS2;
            SHOW_REG_AFTER_TWO_REGS ("^");
        }
    );

    return 0;

}

char ImplXorI   (CPU* CPU, const ImmValue  imm, const RegNumber numRs1, const RegNumber numRd) {
    
    IMPL 
    (
        {
            SHOW_REG_BEFORE ("xorI");
            CHECK_RD_X0;
            RD = (RegValue)((ImmValue)RS1 ^ imm);
            SHOW_REG_AFTER_IMM_REG ("^");
        }
    );

}

//=========================================================================
//============================COMPARISON OPERS=============================
//=========================================================================
char ImplSltI (CPU* CPU, const ImmValue  imm, const RegNumber numRs1, const RegNumber numRd) {

    IMPL 
    (
        {
            SHOW_REG_BEFORE ("sltI");
            CHECK_RD_X0;
            if ((SignedRegValue) (RS1) < (SignedRegValue)imm)
                RD = 1;
            else    
                RD = 0;
            SHOW_REG_AFTER_IMM_REG ("<(signed)");
        }
    );

}

char ImplSltIU (CPU* CPU, const ImmValue  imm, const RegNumber numRs1, const RegNumber numRd) {

    IMPL 
    (
        {
            SHOW_REG_BEFORE ("sltIU");
            CHECK_RD_X0;
            if (RS1 < (RegValue)imm)
                RD = 1;
            else    
                RD = 0;
            SHOW_REG_AFTER_IMM_REG ("<(unsigned)");
        }
    );

}

//=========================================================================
//=================================SHIFTS==================================
//=========================================================================
char ImplSllI (CPU* CPU, const ImmValue  imm, const RegNumber numRs1, const RegNumber numRd) {

    IMPL 
    (
        {
            SHOW_REG_BEFORE ("sllI");
            CHECK_RD_X0;
            RD = RS1 << imm;  

        }
    );

}

char ImplSrlI (CPU* CPU, const ImmValue  imm, const RegNumber numRs1, const RegNumber numRd) {

    IMPL 
    (
        {
            SHOW_REG_BEFORE ("srlI");
            CHECK_RD_X0;
            RD = RS1 >> imm;

        }
    );

}

char ImplSraI (CPU* CPU, const ImmValue  imm, const RegNumber numRs1, const RegNumber numRd) {

    IMPL 
    (
        {
            SHOW_REG_BEFORE ("sraI");
            CHECK_RD_X0;
            RD = (RegValue)((SignedRegValue) RS1 >> imm);
            SHOW_REG_AFTER_IMM_REG (">>");
        }
    );
    
}

//=========================================================================
//=============================MEMORY OPERS================================
//=========================================================================
char ImplLb    (CPU* CPU, const ImmValue imm, const RegNumber numRs1, const RegNumber numRd) {
    
    IMPL 
    (
        {
            SHOW_REG_BEFORE ("lb");
            CHECK_RD_X0;
            RD = (RegValue)((OneByte)(MEMORY [(ImmValue)RS1 + imm]));

        }
    );

}

char ImplLh    (CPU* CPU, const ImmValue imm, const RegNumber numRs1, const RegNumber numRd) {

    IMPL
    (
        {
            SHOW_REG_BEFORE ("lh");
            CHECK_RD_X0;
            RD = (RegValue) ((TwoBytes)(MEMORY [(ImmValue)RS1 + imm]));

        }
    );

}

char ImplLw    (CPU* CPU, const ImmValue imm, const RegNumber numRs1, const RegNumber numRd) {

    IMPL
    (
        {
            SHOW_REG_BEFORE ("lw");
            CHECK_RD_X0;
            RD = (RegValue) ((FourBytes)(MEMORY [(ImmValue)RS1 + imm]));

        }
    );

}

char ImplSb    (CPU* CPU, const ImmValue imm, const RegNumber numRs1, const RegNumber numRs2) { 

    IMPL
    (
        {
            MEMORY [(ImmValue)RS1 + imm] = (OneByte)RS2;

        }
    );

}

char ImplSh    (CPU* CPU, const ImmValue  imm, const RegNumber numRs1, const RegNumber numRs2) { 

    IMPL
    (
        {
            MEMORY [(ImmValue)RS1 + imm] = (TwoBytes)RS2;

        }
    );

}

char ImplSw   (CPU* CPU, const ImmValue imm, const RegNumber numRs1, const RegNumber numRs2) { 

    IMPL
    (
        {
            MEMORY [(ImmValue)RS1 + imm] = (FourBytes)RS2;

        }
    );

}

//=========================================================================
//================================LUI?===================================== (Vuitton)
//=========================================================================
char ImplLui   (CPU* CPU, const ImmValue imm, const RegNumber numRd) {

    IMPL
    (
        {
            SHOW_REG_BEFORE ("lui");
            CHECK_RD_X0;
            RD = (RegValue)(imm << 12);
        }
    );

}

//=========================================================================
//==========================UNCONDITIONAL JUMPS============================
//=========================================================================
char ImplJal   (CPU* CPU, const ImmValue imm, const RegNumber numRd) {

    IMPL
    (
        {
            CHECK_RD_X0;
            RD = PC + 4;
            PC = (RegValue)((ImmValue)PC + imm);
        }
    );

}

char ImplJalR  (CPU* CPU, const ImmValue imm, const RegNumber numRs1, const RegNumber numRd) {

    IMPL
    (
        {
            CHECK_RD_X0;
            RD = PC + 4;
            PC = (RegValue)((((ImmValue)RS1 + imm) >> 1) << 1);
            PC = PC & ~(1u << 0);

        }
    );

}

//=========================================================================
//===========================CONDITIONAL JUMPS=============================
//=========================================================================
char ImplBeq   (CPU* CPU, const ImmValue imm, const RegNumber numRs1, const RegNumber numRs2) {

    IMPL
    (
        {

            if (RS1 == RS2) {
                PC = (RegValue)((ImmValue)PC + imm);
                return 0;
            }

        }
    );

}

char ImplBne   (CPU* CPU, const ImmValue imm, const RegNumber numRs1, const RegNumber numRs2) {

    IMPL
    (
        {

            if (RS1 != RS2) {
                PC = (RegValue)((ImmValue)PC + imm);
                return 0;
            }

        }
    );
    
}

char ImplBlt   (CPU* CPU, const ImmValue imm, const RegNumber numRs1, const RegNumber numRs2) {

    IMPL
    (
        {

            if ((SignedRegValue) RS1 < (SignedRegValue) RS2) {
                PC = (RegValue)((ImmValue)PC + imm);
                return 0;
            }

        }
    );

}

char ImplBge   (CPU* CPU, const ImmValue imm, const RegNumber numRs1, const RegNumber numRs2) {

    IMPL
    (
        {

            if ((SignedRegValue) RS1 >= (SignedRegValue) RS2) {
                PC = (RegValue)((ImmValue)PC + imm);
                return 0;
            }

        }
    );

}

char ImplBltU   (CPU* CPU, const ImmValue imm, const RegNumber numRs1, const RegNumber numRs2) {

    IMPL
    (
        {

            if (RS1 < RS2) {
                PC = (RegValue)((ImmValue)PC + imm);
                return 0;
            }

        }
    );

}

char ImplBgeU   (CPU* CPU, const ImmValue imm, const RegNumber numRs1, const RegNumber numRs2) { //kuda bezhish
    
    IMPL
    (
        {

            if (RS1 >= RS2) {
                PC = (RegValue)((ImmValue)PC + imm);
                return 0;
            }

        }
    );

}
