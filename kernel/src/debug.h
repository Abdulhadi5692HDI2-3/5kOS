#pragma once

#include <stdint.h>

#define _INIT_DEBUG

#define _BUGCHECK
#define KeAssert(cond) KeAssertEx(cond, __FILE__, __LINE__)

#ifdef _BUGCHECK
// causes a bugcheck (like a bsod in windows)
void KeBugCheck(const char* string, ...);
#endif

// Assert
void KeAssertEx(bool condition, const char* file, int line);