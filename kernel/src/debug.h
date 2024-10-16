#pragma once

#include <stdint.h>

#define KeAssert(cond) KeAssertEx(cond, __FILE__, __LINE__)

// causes a bugcheck (like a bsod in windows)
void KeBugCheck(const char* string, ...);
// Assert
void KeAssertEx(bool condition, const char* file, int line);