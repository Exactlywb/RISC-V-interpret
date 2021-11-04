#ifndef INTERPRET_H__
#define INTERPRET_H__

#include "../common.h"

typedef enum Opcodes {

    firstOpGroup  = 0b0110011,  //add, sub, xor, or, and
    secondOpGroup = 0b0010011   //addi, slti, sltiu, xori, ori, andi, slli, srli, srai
    
} Opcodes;

off_t   GetFileSize     (const int fd);
int     RunInterpret    (const int input);

#endif
