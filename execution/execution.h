#ifndef EXEC_H__
#define EXEC_H__

#include "physmemory.h"

typedef uint32_t    PhysAddr;
typedef uint32_t    VirtAddr;
typedef int32_t     SignedRegValue;
typedef uint32_t    RegValue;
typedef int32_t     ImmValue;
typedef uint8_t     RegNumber;

typedef uint32_t    ProgCounter;

#define REG_NUM 32

typedef struct CPU {

    PhysMemory  memory [PHYS_MEM_SIZE];
    RegValue    registers [REG_NUM];
    ProgCounter pc;
    
} CPU;

//=========================================================================
//================================ARITHMETIC===============================
//=========================================================================
char ImplAdd  (CPU* CPU, const RegNumber numRs1,    const RegNumber numRs2, const RegNumber numRd);
char ImplAddI (CPU* CPU, const ImmValue  imm,       const RegNumber numRs1, const RegNumber numRd);
char ImplSub  (CPU* CPU, const RegNumber numRs1,    const RegNumber numRs2, const RegNumber numRd);

//=========================================================================
//==============================LOGICAL OPERS==============================
//=========================================================================
char ImplAnd   (CPU* CPU, const RegNumber numRs1,   const RegNumber numRs2, const RegNumber numRd);
char ImplAndI  (CPU* CPU, const ImmValue  imm,      const RegNumber numRs1, const RegNumber numRd);
char ImplOr    (CPU* CPU, const RegNumber numRs1,   const RegNumber numRs2, const RegNumber numRd);
char ImplOrI   (CPU* CPU, const ImmValue  imm,      const RegNumber numRs1, const RegNumber numRd);
char ImplXor   (CPU* CPU, const RegNumber numRs1,   const RegNumber numRs2, const RegNumber numRd);
char ImplXorI  (CPU* CPU, const ImmValue  imm,      const RegNumber numRs1, const RegNumber numRd);

//=========================================================================
//============================COMPARISON OPERS=============================
//=========================================================================
char ImplSltI  (CPU* CPU, const ImmValue  imm,      const RegNumber numRs1, const RegNumber numRd);
char ImplSltIU (CPU* CPU, const ImmValue  imm,      const RegNumber numRs1, const RegNumber numRd); 

//=========================================================================
//=================================SHIFTS==================================
//=========================================================================
char ImplSllI  (CPU* CPU, const ImmValue  imm,      const RegNumber numRs1, const RegNumber numRd);
char ImplSrlI  (CPU* CPU, const ImmValue  imm,      const RegNumber numRs1, const RegNumber numRd);
char ImplSraI  (CPU* CPU, const ImmValue  imm,      const RegNumber numRs1, const RegNumber numRd);

//=========================================================================
//=============================MEMORY OPERS================================
//=========================================================================
char ImplLb    (CPU* CPU, const ImmValue  imm,      const RegNumber numRs1, const RegNumber numRd);
char ImplLh    (CPU* CPU, const ImmValue  imm,      const RegNumber numRs1, const RegNumber numRd);
char ImplLw    (CPU* CPU, const ImmValue  imm,      const RegNumber numRs1, const RegNumber numRd);
char ImplSb    (CPU* CPU, const ImmValue  imm,      const RegNumber numRs1, const RegNumber numRs2);
char ImplSh    (CPU* CPU, const ImmValue  imm,      const RegNumber numRs1, const RegNumber numRs2);
char ImplSw    (CPU* CPU, const ImmValue  imm,      const RegNumber numRs1, const RegNumber numRs2);

//=========================================================================
//================================LUI?===================================== (Vuitton)
//=========================================================================
char ImplLui   (CPU* CPU, const ImmValue  imm,      const RegNumber numRd);

//=========================================================================
//==========================UNCONDITIONAL JUMPS============================
//=========================================================================
char ImplJal   (CPU* CPU, const ImmValue  imm,      const RegNumber numRd);
char ImplJalR  (CPU* CPU, const ImmValue  imm,      const RegNumber numRs1, const RegNumber numRd);

//=========================================================================
//===========================CONDITIONAL JUMPS=============================
//=========================================================================
char ImplBeq   (CPU* CPU, const ImmValue  imm,      const RegNumber numRs1, const RegNumber numRs2);
char ImplBne   (CPU* CPU, const ImmValue  imm,      const RegNumber numRs1, const RegNumber numRs2);
char ImplBlt   (CPU* CPU, const ImmValue  imm,      const RegNumber numRs1, const RegNumber numRs2);
char ImplBge   (CPU* CPU, const ImmValue  imm,      const RegNumber numRs1, const RegNumber numRs2);
char ImplBltU  (CPU* CPU, const ImmValue  imm,      const RegNumber numRs1, const RegNumber numRs2);
char ImplBgeU  (CPU* CPU, const ImmValue  imm,      const RegNumber numRs1, const RegNumber numRs2);

#endif
