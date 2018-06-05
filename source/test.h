#ifndef C_TEST_H
#define C_TEST_H

#include "core_utils.h"

// need extern "C" in shared (c/c++) 
// headers or in .cpp files that define a 
// function referenced in a shared header 
// (that is going to be called from a .c file)

#ifdef __cplusplus
extern "C" 
{
#endif

void c_func(void);

#ifdef __cplusplus
}
#endif

#endif
