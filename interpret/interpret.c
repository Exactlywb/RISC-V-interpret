#include "interpret.h"

static      int     RunParser       (const char* buffer, const off_t bufferSize);
static      int     HandleCommand   (const unsigned char* command);

int RunInterpret (const int input) {

    SECURITY_FUNCTION (input < 0, {printf ("Bad input file descriptor!\n");}, input);

    off_t fileSize = GetFileSize (input);
    SECURITY_FUNCTION (fileSize <= 0, {}, (int)fileSize);

    char* buffer = (char*)calloc ((size_t)fileSize, sizeof (char));
    SECURITY_FUNCTION (IS_NULL (buffer), {printf ("Bad alloc for char* buffer in function %s\n"
                                                    , __func__);}, -1);

    int readErr = read (input, buffer, (size_t)fileSize);
    SECURITY_FUNCTION (readErr < 0, {free (buffer);
                                     perror ("Bad read from input file");}, errno);

    int parseErr = RunParser (buffer, fileSize);
    SECURITY_FUNCTION (parseErr != 0, {free (buffer);}, parseErr);

    free (buffer);

    return 0;

}

#define COMMAND_SIZE 4

static int RunParser (const char* buffer, const off_t bufferSize) {

    for (off_t curBufInd = 0; curBufInd < bufferSize; curBufInd += 4)
        HandleCommand ((unsigned char*)(buffer + curBufInd));

    return 0;

}

#define CALC_RD (command [COMMAND_SIZE - 1] >> 7) +         \
                (command [COMMAND_SIZE - 2] & 0b00001111) * 2

#define CALC_MAGIC_CONST command [COMMAND_SIZE - 2] & 0b01110000

#define CALC_RS1    (command [COMMAND_SIZE - 2] >> 7) +     \
                    (command [COMMAND_SIZE - 3] & 0b00001111) * 2

#define CALC_IMM    (command [COMMAND_SIZE - 3] >> 4) +     \
                    (command [COMMAND_SIZE - 4] & 0b01111111) * 16

#define CALC_RS2    (command [COMMAND_SIZE - 3] >> 4)

#define CALC_FRONT_MAGIC_CONST command [COMMAND_SIZE - 4]

static int FirstOpGroupHandle   (const int frontConst, const int rs2, const int rs1,
                                const int middleConst, const int rd) { //add, sub, xor, or, and
    
    switch (middleConst) {

        case 0b000: { //add, sub

            if (frontConst == 0)
                return add (...);
            else if (frontConst == 32)
                return sub (...);
            
            return -1;

        }
        case 0b100: //xor
            return xor (...);
        case 0b110: //or
            return or (...);
        case 0b111: //and
            return and (...);

    }
    
    return -1;

}
//!TODO check signed nums

static int HandleShifts (const int frontConst, const int shamt, const int rs1,
                        const int middleConst, const int rd) {

    if (middleConst == 0b001) //slli
        return slli (...);
    else if (middleConst == 0b101) { //srli or srai

        if (frontConst == 0) //srli
            return srli (...);
        else if (frontConst == 32) //srai
            return srai (...);

    }

    return -1;

}

static int HandleArithWithInt (const int imm, const int rs1, const int middleConst, const int rd) {

    switch (middleConst) {

        case 0b000:
            return addi (...);
        case 0b010:
            return slti (...);
        case 0b011:
            return sltiu (...);
        case 0b100:
            return xori (...);
        case 0b110:
            return ori (...);
        case 0b111:
            return andi (...);
        default:
            return -1;

    }

}

static int SecondOpGroupHandle (const unsigned char* command, const int rs1,
                                const int middleConst, const int rd) {

    if (middleConst == 0b101 || middleConst == 0b001) { //slli, srli, srai

        int shamt       = CALC_RS2;
        int frontConst  = CALC_FRONT_MAGIC_CONST;

        return HandleShifts (frontConst, shamt, rs1, middleConst, rd);

    }

    int imm = CALC_IMM;

    return HandleArithWithInt (imm, rs1, middleConst, rd);
    
}

static int HandleCommand (const unsigned char* command) {

    int opcode = (*(command + COMMAND_SIZE - 1)) & 0b01111111;

    switch (opcode) {

        case firstOpGroup: { //add, sub, xor, or, and
            
            int rd              = CALC_RD;

            int magicConst      = CALC_MAGIC_CONST;
            int rs1             = CALC_RS1;
            int rs2             = CALC_RS2;
            
            int frontMagicConst = CALC_FRONT_MAGIC_CONST;

            return FirstOpGroupHandle (frontMagicConst, rs2, rs1, magicConst, rd);

        }
        case secondOpGroup: {   //addi, slti, sltiu, xori, ori, andi, slli, srli, srai

            int rd              = CALC_RD;
            int magicConst      = CALC_MAGIC_CONST;
            int rs1             = CALC_RS1;

            return SecondOpGroupHandle (command, rs1, magicConst, rd);

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
