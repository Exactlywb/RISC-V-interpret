#ifndef INTERPRET_H__
#define INTERPRET_H__

#include "../common.h"

typedef enum Opcodes {

    firstOpGroup    = 0b0110011,    //add, sub, xor, or, and
    secondOpGroup   = 0b0010011,    //addi, slti, sltiu, xori, ori, andi, slli, srli, srai
    thirdOpGroup    = 0b0000011,    //lb, lh, lw
    fourthOpGroup   = 0b0100011     //sb, sh, sw
    
} Opcodes;

off_t   GetFileSize     (const int fd);
int     RunInterpret    (const int input);

#endif
