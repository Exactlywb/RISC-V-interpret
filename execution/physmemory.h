#ifndef PHYSMEMORY_H__
#define PHYSMEMORY_H__

#include "../common.h"

#define PHYS_MEM_SIZE 1024

typedef int8_t  OneByte;
typedef int16_t TwoBytes;
typedef int32_t FourBytes;


typedef struct PhysMemory {

    char buffer [PHYS_MEM_SIZE];

} PhysMemory;

#endif
