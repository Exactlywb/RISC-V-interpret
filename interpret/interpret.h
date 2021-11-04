#ifndef INTERPRET_H__
#define INTERPRET_H__

#include "../common.h"
#include "../execution/execution.h"

typedef enum Opcodes {

    firstOpGroup    = 0b0110011,    //add, sub, xor, or, and
    secondOpGroup   = 0b0010011,    //addi, slti, sltiu, xori, ori, andi, slli, srli, srai
    thirdOpGroup    = 0b0000011,    //lb, lh, lw
    fourthOpGroup   = 0b0100011,    //sb, sh, sw
    fifthOpGroup    = 0b0110111,    //lui
    sixthOpGroup    = 0b1101111,    //jal
    seventhOpGroup  = 0b1100111,    //jalr
    eightthOpGroup  = 0b1100011     //beq, bne, blt, bge, bltu, bgeu
    
} Opcodes;

off_t   GetFileSize     (const int fd);
int     RunInterpret    (const int input);

#endif
