#ifndef COMMON_H__
#define COMMON_H__

#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>

#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <errno.h>

#define IS_NULL(ptr) (!ptr)

#define SECURITY_FUNCTION(condition, toDoB4Return, retVal)      do {                                        \
                                                                                                            \
                                                                    if (condition) {                        \
                                                                                                            \
                                                                        toDoB4Return                        \
                                                                        return retVal;                      \
                                                                                                            \
                                                                    }                                       \
                                                                                                            \
                                                                } while (0)\


#endif
