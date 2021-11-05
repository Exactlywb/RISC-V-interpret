#include "interpret.h"

static      int     RunParser       (const unsigned char* buffer, const off_t bufferSize, State *mainState);
static      int     HandleCommand   (const unsigned char* command, State *mainState);

int RunInterpret (const int input) {

    SECURITY_FUNCTION (input < 0, {printf ("Bad input file descriptor!\n");}, input);

    off_t fileSize = GetFileSize (input);
    SECURITY_FUNCTION (fileSize <= 0, {}, (int)fileSize);

    unsigned char* buffer = (unsigned char*)calloc ((size_t)fileSize, sizeof (*buffer));
    SECURITY_FUNCTION (IS_NULL (buffer), {printf ("Bad alloc for char* buffer in function %s\n"
                                                    , __func__);}, -1);

    int readErr = read (input, buffer, (size_t)fileSize);
    SECURITY_FUNCTION (readErr < 0, {free (buffer);
                                     perror ("Bad read from input file");}, errno);

    State mainState = {};

    int parseErr = RunParser (buffer, fileSize, &mainState);
    SECURITY_FUNCTION (parseErr != 0, {free (buffer);}, parseErr);

    free (buffer);

    return 0;

}

#define COMMAND_SIZE 4

static int RunParser (const unsigned char* buffer, const off_t bufferSize, State *mainState) {

    for (off_t curBufInd = 0; curBufInd < bufferSize; curBufInd += 4)
        HandleCommand (buffer + curBufInd, mainState);

    return 0;

}

static int32_t GetBites (const unsigned char* command, const int start, const int end) {
    //!TODO maybe there's the opportunity to optimize this function

    int32_t res = 0;

    for (int i = start; i <= end; i++)
        res |= ((command [3 - i / 8] >> (i % 8)) & 1) << (i - start);

    return res;

}

static int FirstOpGroupHandle   (const uint32_t frontConst, const uint32_t rs2, const uint32_t rs1,
                                const uint32_t middleConst, const uint32_t rd, State* state) { //add, sub, xor, or, and

    switch (middleConst) {

        case 0b000: { //add, sub

            if (frontConst == 0)
                return ImplAdd (state, rs1, rs2, rd);
            else if (frontConst == 32)
                return ImplSub (state, rs1, rs2, rd);
            
            return -1;

        }
        case 0b100: //xor
            return ImplXor  (state, rs1, rs2, rd);
        case 0b110: //or
            return ImplOr   (state, rs1, rs2, rd);
        case 0b111: //and
            return ImplAnd  (state, rs1, rs2, rd);

    }
    
    return -1;

}

static int HandleShifts (const uint32_t frontConst, const uint32_t shamt, const uint32_t rs1,
                        const uint32_t middleConst, const uint32_t rd, State* state) {

    if (middleConst == 0b001) //slli
        return ImplSllI (state, shamt, rs1, rd);
    else if (middleConst == 0b101) { //srli or srai

        if (frontConst == 0)//srli 
            return ImplSrlI (state, shamt, rs1, rd);
        else if (frontConst == 32) //srai
            return ImplSraI (state, shamt, rs1, rd);

    }

    return -1;

}

static int HandleArithWithInt   (const uint32_t imm, const uint32_t rs1, 
                                const uint32_t middleConst, const uint32_t rd, State* state) {
    
    char highestBit = (imm & (1 << 11)) >> 11;
    printf ("highestBit = %d\n", highestBit);
    int extendedImm = imm;
    for (int i = 0; i < 31; i++) {
        printf ("%d ", (imm & (1 << (31 - i))) >> (31 - i));
        extendedImm |= imm & (1 << (31 - i));
    }

    for (int i = 12; i < 32; i++)
        extendedImm ^= (highestBit << i);

    printf ("extendedImm = %d\n", extendedImm);

    // int32_t highestBit = (imm >> 11) & 1;
    // int32_t extendedImm = imm ^ (highestBit << 11);
    // extendedImm |= (highestBit << 31);

    printf ("extended %d\n", extendedImm);
    
    switch (middleConst) {

        case 0b000:
            printf ("addi\n");
            return ImplAddI     (state, imm, rs1, rd);
        case 0b010:
            return ImplSltI     (state, imm, rs1, rd);
        case 0b011:
            return ImplSltIU    (state, imm, rs1, rd);
        case 0b100:
            return ImplXorI     (state, imm, rs1, rd);
        case 0b110:
            return ImplOrI      (state, imm, rs1, rd);
        case 0b111:
            return ImplAddI     (state, imm, rs1, rd);
        default:
            printf ("Unexpected middle constant in function %s\n", __func__);
            return -1;

    }

}

static int SecondOpGroupHandle (const unsigned char* command, const uint32_t rs1,
                                const uint32_t middleConst, const uint32_t rd, State* state) {

    if (middleConst == 0b101 || middleConst == 0b001) { //slli, srli, srai

        uint32_t shamt       = GetBites (command, 20, 24);
        uint32_t frontConst  = GetBites (command, 25, 31);

        return HandleShifts (frontConst, shamt, rs1, middleConst, rd, state);

    }

    ImmValue imm = GetBites (command, 20, 31);

    return HandleArithWithInt (imm, rs1, middleConst, rd, state);
    
}

static int ThirdOpGroupHandle   (const uint32_t imm, const uint32_t rs1, const uint32_t middleConst, 
                                const uint32_t rd, State* state) {

    switch (middleConst) {

        case 0b000:
            return ImplLb (state, imm, rs1, rd);
        case 0b001:
            return ImplLh (state, imm, rs1, rd);
        case 0b010:
            return ImplLw (state, imm, rs1, rd);

    }

    return -1;

}

static int FourthOpGroupHandle (const uint32_t frontImm, const uint32_t rs2, const uint32_t rs1,
                                const uint32_t middleConst, const uint32_t lastImm, State* state) {

    switch (middleConst) { //!TODO

        case 0b000:
            return ImplSb (state, lastImm, rs1, rs2);
        case 0b001:
            return ImplSh (state, lastImm, rs1, rs2);
        case 0b010:
            return ImplSw (state, lastImm, rs1, rs2);

    }

    return -1;

}

static int BranchOpsHandle (const uint32_t frontImm, const uint32_t rs2, const uint32_t rs1,
                            const uint32_t middleConst, const uint32_t lastImm) {

    switch (middleConst) { //!TODO

        case 0b000:
            printf ("beq\n");//return beq (...);
            return 0;
        case 0b001:
            printf ("bne\n");//return bne (...);
            return 0;
        case 0b100:
            printf ("blt\n");//return blt (...);
            return 0;
        case 0b101:
            printf ("bge\n");//return bge (...);
            return 0;
        case 0b110:
            printf ("bltu\n");//return bltu (...);
            return 0;
        case 0b111:
            printf ("bgeu\n");//return bgeu (...);
            return 0;

    }

    return -1;

}

static int HandleCommand (const unsigned char* command, State *state) {

    uint32_t opcode = GetBites (command, 0, 6);
    printf ("opcode = %d\n", opcode);

    switch (opcode) {

        case firstOpGroup: { //add, sub, xor, or, and
            
            uint32_t rd              = GetBites (command, 7, 11);

            uint32_t magicConst      = GetBites (command, 12, 14);
            uint32_t rs1             = GetBites (command, 15, 19);
            uint32_t rs2             = GetBites (command, 20, 24);
            
            uint32_t frontMagicConst = GetBites (command, 25, 31);

            return FirstOpGroupHandle (frontMagicConst, rs2, rs1, magicConst, rd, state);

        }
        case secondOpGroup: {   //addi, slti, sltiu, xori, ori, andi, slli, srli, srai

            uint32_t rd          = GetBites (command, 7, 11);
            uint32_t magicConst  = GetBites (command, 12, 14);
            uint32_t rs1         = GetBites (command, 15, 19);

            return SecondOpGroupHandle (command, rs1, magicConst, rd, state);

        }
        case thirdOpGroup: {    //lb, lh, lw

            uint32_t rd          = GetBites (command, 7, 11);
            uint32_t magicConst  = GetBites (command, 12, 14);
            uint32_t rs1         = GetBites (command, 15, 19);

            uint32_t imm         = GetBites (command, 20, 31);

            return ThirdOpGroupHandle (imm, rs1, magicConst, rd, state);

        }
        case fourthOpGroup: {   //sb, sh, sw

            uint32_t lastImm     = GetBites (command, 7, 11);
            
            uint32_t magicConst  = GetBites (command, 12, 14);

            uint32_t rs1         = GetBites (command, 15, 19);
            uint32_t rs2         = GetBites (command, 20, 24);

            uint32_t frontImm    = GetBites (command, 25, 31);

            return FourthOpGroupHandle (frontImm, rs2, rs1, magicConst, lastImm, state);

        }
        case fifthOpGroup: { //lui

            uint32_t rd  = GetBites (command, 7, 11);
            uint32_t imm = GetBites (command, 12, 31);

            return ImplLui (state, imm, rd);

        }
        case sixthOpGroup: { //jal

            uint32_t rd  = GetBites (command, 7, 11);
            uint32_t imm = GetBites (command, 12, 31);

            return ImplJal (state, imm, rd);

        }
        case seventhOpGroup: { //jalr

            uint32_t rd          = GetBites (command, 7, 11);
            // uint32_t magicConst  = GetBites (command, 12, 14);
            uint32_t rs1         = GetBites (command, 15, 19);

            uint32_t imm         = GetBites (command, 20, 31);

            return ImplJalR (state, imm, rs1, rd);

        }
        case eightthOpGroup: { //beq, bne, blt, bge, bltu, bgeu

            uint32_t lastImm     = GetBites (command, 7, 11);
            uint32_t middleConst = GetBites (command, 12, 14);
            
            uint32_t rs1         = GetBites (command, 15, 19);
            uint32_t rs2         = GetBites (command, 20, 24);

            uint32_t frontImm    = GetBites (command, 25, 31);

            return BranchOpsHandle (frontImm, rs2, rs1, middleConst, lastImm);
            
        }
        default:
            printf ("Unexpected opcode %d\n", opcode);
            return -1;

    }

    return 0;

}

off_t GetFileSize (const int fd) {

    off_t res = lseek (fd, 0, SEEK_END);
    SECURITY_FUNCTION (res < 0, {perror ("Bad execution for lseek");}, (off_t)errno);

    off_t lseekErr = lseek (fd, 0, SEEK_SET);
    SECURITY_FUNCTION (lseekErr < 0, {perror ("Bad execution for lseek");}, (off_t)errno);

    return res;

}
