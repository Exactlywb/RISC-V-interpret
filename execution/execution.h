#ifndef EXEC_H__
#define EXEC_H__

#include "physmemory.h"

typedef uint32_t    PhysAddr;
typedef uint32_t    VirtAddr;
typedef int32_t     SignedRegValue;
typedef uint32_t    RegValue;
typedef int32_t     ImmValue;
typedef uint8_t     RegNumber;

typedef uint32_t ProgCounter;

typedef struct State {

    PhysMemory  Memory;
    RegValue    Registors[32];
    ProgCounter pc;
    
} State; 

//-------------------------------------------------------------------------
//--------------------------------ARITHMETIC-------------------------------
//-------------------------------------------------------------------------
char ImplAdd  (State* state, RegNumber numRs1, RegNumber numRs2, RegNumber numRd);
char ImplAddI (State* state, ImmValue  imm,    RegNumber numRs1, RegNumber numRd);
char ImplSub  (State* state, RegNumber numRs1, RegNumber numRs2, RegNumber numRd);

//-------------------------------------------------------------------------
//------------------------------LOGICAL OPERS------------------------------
//-------------------------------------------------------------------------
char ImplAnd   (State* state, RegNumber numRs1, RegNumber numRs2, RegNumber numRd);
char ImplAndI  (State* state, ImmValue  imm,    RegNumber numRs1, RegNumber numRd);
char ImplOr    (State* state, RegNumber numRs1, RegNumber numRs2, RegNumber numRd);
char ImplOrI   (State* state, ImmValue  imm,    RegNumber numRs1, RegNumber numRd);
char ImplXor   (State* state, RegNumber numRs1, RegNumber numRs2, RegNumber numRd);
char ImplXorI  (State* state, ImmValue  imm,    RegNumber numRs1, RegNumber numRd);

//-------------------------------------------------------------------------
//----------------------------COMPARISON OPERS-----------------------------
//-------------------------------------------------------------------------
char ImplSltI  (State* state, ImmValue  imm,    RegNumber numRs1, RegNumber numRd);
char ImplSltIU (State* state, ImmValue  imm,    RegNumber numRs1, RegNumber numRd); 

//-------------------------------------------------------------------------
//---------------------------------SHIFTS----------------------------------
//-------------------------------------------------------------------------
char ImplSllI  (State* state, ImmValue  imm,    RegNumber numRs1, RegNumber numRd);
char ImplSrlI  (State* state, ImmValue  imm,    RegNumber numRs1, RegNumber numRd);
char ImplSraI  (State* state, ImmValue  imm,    RegNumber numRs1, RegNumber numRd);

//-------------------------------------------------------------------------
//-----------------------------MEMORY OPERS--------------------------------
//-------------------------------------------------------------------------
char ImplLb    (State* state, ImmValue  imm,    RegNumber numRs1, RegNumber numRd);
char ImplLh    (State* state, ImmValue  imm,    RegNumber numRs1, RegNumber numRd);
char ImplLw    (State* state, ImmValue  imm,    RegNumber numRs1, RegNumber numRd);
char ImplSb    (State* state, ImmValue  imm,    RegNumber numRs1, RegNumber numRs2);
char ImplSh    (State* state, ImmValue  imm,    RegNumber numRs1, RegNumber numRs2);
char ImplSw    (State* state, ImmValue  imm,    RegNumber numRs1, RegNumber numRs2);

//-------------------------------------------------------------------------
//---------------------------------LUI-------------------------------------
//-------------------------------------------------------------------------
char ImplLui    (State* state, ImmValue  imm,   RegNumber numRd);

//-------------------------------------------------------------------------
//--------------------------UNCONDITIONAL JUMPS----------------------------
//-------------------------------------------------------------------------
char ImplJal    (State* state, ImmValue  imm,   RegNumber numRd);
char ImplJalR   (State* state, ImmValue  imm,   RegNumber numRs1, RegNumber numRd);


#define ZERO_POINTER                    \
                        if (!state)     \
                            return 1;

#endif
