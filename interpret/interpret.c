#include "../common.h"
#include "interpret.h"

//Main functions
static  char    HandleCommands  (const char* command, CPU* CPU);
static  int     StartParsing    (const off_t bufferSize, CPU *CPU);

//Common
static  int         ReadBuffer      (const int inputFile, const off_t fileSize, CPU* CPU);
        off_t       GetFileSize     (const int fd);
static  int         ReadBuffer      (const int inputFile, const off_t fileSize, CPU* CPU);
static  ImmValue    GetBits         (const char* command, const ImmValue start, const ImmValue end);
static  ImmValue    ExtendedImm     (const ImmValue imm, char startBit);

//Commands group handling
static int HandleCommandsGroup1     (const  ImmValue    midConst,   const   RegNumber rs2,  const   RegNumber rs1,
                                    const   ImmValue    frontConst, const   RegNumber rd,           CPU* CPU);
static int ShiftRecognizer          (const  ImmValue    frontConst, const   ImmValue shamt, const   RegNumber rs1,
                                    const   ImmValue    midConst,   const   RegNumber rd,           CPU* CPU);
static int HandleArithm     (const  ImmValue    imm,        const   RegNumber rs1, 
                                    const   ImmValue    midConst,   const   RegNumber rd,           CPU* CPU);
static int HandleCommandsGroup2     (const  char*       command,    const   RegNumber rs1,
                                    const   ImmValue    midConst,   const   RegNumber rd,           CPU* CPU);
static int HandleCommandsGroup3     (const  ImmValue    imm,        const   RegNumber rs1,  const   ImmValue midConst, 
                                    const   RegNumber   rd,                 CPU*    CPU);
static int HandleCommandsGroup4     (const  ImmValue    imm,        const   RegNumber rs2,  const   RegNumber rs1,
                                    const   ImmValue    midConst,           CPU*    CPU);
static int BranchesHandle           (CPU    *CPU,                   const   ImmValue imm,   const   RegNumber rs2,
                                    const RegNumber rs1,            const   ImmValue midConst);

int RunInterpret (const int inputFile) {

    SECURITY_FUNCTION (inputFile < 0, {perror ("Bad input file descriptor");}, errno);

    off_t fileSize = GetFileSize (inputFile);
    SECURITY_FUNCTION (fileSize > PHYS_MEM_SIZE, {printf ("Too big program!\n");}, -1);

    CPU CPU = {};
    int err = ReadBuffer (inputFile, fileSize, &CPU);
    SECURITY_FUNCTION (err != 0, {}, err);

    err = StartParsing (fileSize, &CPU);
    SECURITY_FUNCTION (err != 0, {}, err);
    
    return 0;
}

static char HandleCommands (const char* command, CPU *CPU) {

    uint8_t opcode = GetBits (command, 0, 6);

    switch (opcode) {

        case firstOpGroup: { //add, sub, xor, or, and
            
            RegNumber rd        = GetBits (command, 7, 11);
            ImmValue midConst   = GetBits (command, 12, 14);

            RegNumber rs1       = GetBits (command, 15, 19);
            RegNumber rs2       = GetBits (command, 20, 24);

            ImmValue frontConst = GetBits (command, 25, 31);

            return HandleCommandsGroup1 (midConst, rs2, rs1, frontConst, rd, CPU);

        }
        case secondOpGroup: {   //addi, slti, sltiu, xori, ori, andi, slli, srli, srai

            RegNumber rd        = GetBits (command, 7, 11);
            ImmValue midConst   = GetBits (command, 12, 14);
            RegNumber rs1       = GetBits (command, 15, 19);

            return HandleCommandsGroup2 (command, rs1, midConst, rd, CPU);

        }
        case thirdOpGroup: {    //lb, lh, lw

            RegNumber rd        = GetBits (command, 7, 11);
            ImmValue midConst   = GetBits (command, 12, 14);
            RegNumber rs1       = GetBits (command, 15, 19);

            ImmValue imm    = GetBits (command, 20, 31);
            imm             = ExtendedImm (imm, 11);

            return HandleCommandsGroup3 (imm, rs1, midConst, rd, CPU);

        }
        case fourthOpGroup: {   //sb, sh, sw

            ImmValue  midConst  = GetBits (command, 12, 14);
            RegNumber rs1       = GetBits (command, 15, 19);
            RegNumber rs2       = GetBits (command, 20, 24);

            ImmValue imm    =  GetBits (command, 7, 11) | GetBits (command, 25, 31)  << 5; 
            imm             = ExtendedImm (imm, 11);

            return HandleCommandsGroup4 (imm, rs2, rs1, midConst, CPU);

        }
        case fifthOpGroup: { //lui

            RegNumber rd    = GetBits (command, 7, 11);

            ImmValue imm    = GetBits (command, 12, 31);
            imm = ExtendedImm (imm, 19);

            return ImplLui (CPU, imm, rd);

        }
        case sixthOpGroup: { //jal

            RegNumber rd     = GetBits (command, 7, 11);
            ImmValue imm     = 0;
            imm             |=  (GetBits (command, 21, 30)  << 1 ) + (GetBits (command, 20, 20)  << 11) + 
                                (GetBits (command, 12, 19)  << 12) + (GetBits (command, 31, 31)  << 20);

            imm = ExtendedImm (imm, 19);

            return ImplJal (CPU, imm, rd);

        }
        case seventhOpGroup: { //jalr

            RegNumber   rd  = GetBits (command, 7, 11);
            RegNumber   rs1 = GetBits (command, 15, 19);

            ImmValue    imm = GetBits (command, 20, 31);
            imm             = ExtendedImm (imm, 11);

            return ImplJalR (CPU, imm, rs1, rd);

        }
        case eightthOpGroup: { //beq, bne, blt, bge, bltu, bgeu
            
            ImmValue imm            = 0;
            ImmValue midConst       = GetBits (command, 12, 14);
            RegNumber rs1           = GetBits (command, 15, 19);
            RegNumber rs2           = GetBits (command, 20, 24);
            
            imm                    |= (GetBits (command, 8, 11) << 1 ) + (GetBits (command, 25, 30)  << 5) + 
                                      (GetBits (command, 7, 7)  << 11) + (GetBits (command, 31, 31)  << 12);
            imm                     = ExtendedImm (imm, 12);

            return BranchesHandle (CPU, imm, rs2, rs1, midConst);

        }
        default:
            printf ("Unknown opcode %d\n", opcode);
            return 1;

    }

    return 0;

}

static int StartParsing (const off_t bufferSize, CPU *CPU) {

    while (CPU->pc < bufferSize) {
        
        int err = HandleCommands ((char*) CPU->memory + CPU->pc, CPU);
        SECURITY_FUNCTION (err != 0, {}, err);

    }

    return 0;
}

//=========================================================================
//========================GROUP COMMANDS HANDLING==========================
//=========================================================================
static int HandleCommandsGroup1 (const ImmValue midConst, const RegNumber rs2, const RegNumber rs1,
                                const ImmValue frontConst, const RegNumber rd, CPU* CPU) { //add, sub, xor, or, and

    
    switch (frontConst) {
                    

        case 0b000: { //add, xor, or, add

            if (midConst == 0)
                return ImplAdd (CPU, rs1, rs2, rd);
            else if (midConst == 4) 
                return ImplXor (CPU, rs1, rs2, rd);
            else if (midConst == 6) 
                return ImplOr  (CPU, rs1, rs2, rd);
            else if (midConst == 7) 
                return ImplAnd (CPU, rs1, rs2, rd);
            
            return -1;
        }
        case 0b100000: //sub
            return ImplSub  (CPU, rs1, rs2, rd);
    }
    return -1;
}

static int ShiftRecognizer (const ImmValue frontConst, const ImmValue shamt, const RegNumber rs1,
                            const ImmValue midConst, const RegNumber rd, CPU* CPU) {

    if (midConst == 0b001) //slli
        return ImplSllI (CPU, shamt, rs1, rd);
    else if (midConst == 0b101) { //srli or srai

        if (frontConst == 0)//srli 
            return ImplSrlI (CPU, shamt, rs1, rd);
        else if (frontConst == 32) //srai
            return ImplSraI (CPU, shamt, rs1, rd);

    }
    
    return -1;

}

static int HandleArithm (const ImmValue imm, const RegNumber rs1, 
                                const ImmValue midConst, const RegNumber rd, CPU* CPU) {
    
    switch (midConst) {

        case 0b000:
            return ImplAddI     (CPU, imm, rs1, rd);
        case 0b010:
            return ImplSltI     (CPU, imm, rs1, rd);
        case 0b011:
            return ImplSltIU    (CPU, imm, rs1, rd);
        case 0b100:
            return ImplXorI     (CPU, imm, rs1, rd);
        case 0b110:
            return ImplOrI      (CPU, imm, rs1, rd);
        case 0b111:
            return ImplAddI     (CPU, imm, rs1, rd);
        default:
            printf ("Unexpected middle constant in function %s\n", __func__);
            return -1;

    }
}

static int HandleCommandsGroup2 (const char* command, const RegNumber rs1,
                                const ImmValue midConst, const RegNumber rd, CPU* CPU) {

    if (midConst == 0b101 || midConst == 0b001) { //slli, srli, srai 

        ImmValue shamt      = GetBits (command, 20, 24);
        ImmValue frontConst = GetBits (command, 25, 31);

        return ShiftRecognizer (frontConst, shamt, rs1, midConst, rd, CPU);
    }

    ImmValue imm    = GetBits (command, 20, 31);
    imm             = ExtendedImm (imm, 11);
    
    return HandleArithm (imm, rs1, midConst, rd, CPU);

}

static int HandleCommandsGroup3 (const ImmValue imm, const RegNumber rs1, const ImmValue midConst, 
                                const RegNumber rd, CPU* CPU) {

    switch (midConst) {

        case 0b000:
            return ImplLb (CPU, imm, rs1, rd);
        case 0b001:
            return ImplLh (CPU, imm, rs1, rd);
        case 0b010:
            return ImplLw (CPU, imm, rs1, rd);

    }
    return -1;
}

static int HandleCommandsGroup4 (const ImmValue imm, const RegNumber rs2, const RegNumber rs1,
                                const ImmValue midConst, CPU* CPU) {

    switch (midConst) {

        case 0b000:
            return ImplSb (CPU, imm, rs1, rs2);
        case 0b001:
            return ImplSh (CPU, imm, rs1, rs2);
        case 0b010:
            return ImplSw (CPU, imm, rs1, rs2);

    }
    return -1;
}

static int BranchesHandle   (CPU *CPU, const ImmValue imm, const RegNumber rs2, const RegNumber rs1,
                            const ImmValue midConst) {

    switch (midConst) {

        case 0b000:
            return ImplBeq  (CPU, imm, rs1, rs2);
        case 0b001:
            return ImplBne  (CPU, imm, rs1, rs2);
        case 0b100:
            return ImplBlt  (CPU, imm, rs1, rs2);
        case 0b101:
            return ImplBge  (CPU, imm, rs1, rs2);
        case 0b110:
            return ImplBltU (CPU, imm, rs1, rs2);
        case 0b111:
            return ImplBgeU (CPU, imm, rs1, rs2);

    }
    return -1;
}

//=========================================================================
//==================================COMMON=================================
//=========================================================================
off_t GetFileSize (const int fd) {

    off_t res = lseek (fd, 0, SEEK_END);
    SECURITY_FUNCTION (res < 0, {perror ("Bad execution for lseek");}, (off_t)errno);

    off_t lseekErr = lseek (fd, 0, SEEK_SET);
    SECURITY_FUNCTION (lseekErr < 0, {perror ("Bad execution for lseek");}, (off_t)errno);

    return res;

}

static int ReadBuffer (const int inputFile, const off_t fileSize, CPU* CPU) {

    int error = read (inputFile, CPU->memory, (size_t)fileSize);
    SECURITY_FUNCTION (error != fileSize, {perror ("Bad read from input file");}, errno);
    
    return 0;
    
}

static ImmValue GetBits (const char* command, const ImmValue start, const ImmValue end) {
    
    ImmValue res = 0;

    for (ImmValue i = start; i <= end; i++)
        res |= ((command [3 - i / 8] >> (i % 8)) & 1) << (i - start); //I know it's terrible...

    return res;

}

static ImmValue ExtendedImm (const ImmValue imm, char startBit) {

    char highestBit = (imm & (1 << startBit)) >> startBit;

    int extendedImm = imm;

    for (int i = startBit + 1; i < 32; i++)
        extendedImm ^= (highestBit << i);

    return extendedImm;

}
