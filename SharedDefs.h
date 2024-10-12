#pragma once
// Shared definitions between the kernel and the bootloader
typedef unsigned long long ulonglong;
typedef ulonglong ULONGLONG;
typedef unsigned int UINT;
typedef UINT _uint;
typedef struct u1 {
	void* Address;
	ULONGLONG BufferSize;
	UINT Width;
	UINT Height;
	UINT PixelsPerScanLine;
} Framebuffer;

typedef struct u2 {
	Framebuffer* bootframebuffer;
} BootParams; // passed to the kernel
