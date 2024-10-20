#pragma once
#include "gnu-efi/inc/efi.h"
#include "kernel/src/acpi/acpi.h"
// Shared definitions between the kernel and the bootloader
typedef unsigned long long ulonglong;
typedef ulonglong ULONGLONG;
typedef unsigned int UINT;
typedef UINT _uint;
typedef unsigned long ULONG; // otherwise aka as size_t
typedef char BYTE; // equivilant to posix's UINT8_T

typedef struct u6 {
	EFI_MEMORY_DESCRIPTOR* Map;
	ULONGLONG MapSize;
	ULONGLONG MapDescriptorSize;
} MemoryMap;
typedef struct u5 {
	void* Address;
	ULONGLONG BufferSize;
	UINT Width;
	UINT Height;
	UINT PixelsPerScanLine;
} Framebuffer;
// Early boot font stuff
#define PSF_MAGIC 0x36
#define PSF_MAGIC1 0x04

typedef struct u4 {
	unsigned char magic[2];
	unsigned char mode;
	unsigned char charsize;
} PSF1_HEADER;

typedef struct u3 {
	PSF1_HEADER* Header;
	void* GlyphBuffer;
} PSF1_FONT;

#define BOOTPARAM_MAGIC 0xFACADE

// Bootparams struct
#ifdef __cplusplus
typedef struct u2 {
	UINT Magic;
	Framebuffer* bootframebuffer;
	PSF1_FONT* bootfont;
	MemoryMap MemMap;
	ACPI::RDSP2* RDSP;
} BootParams; // passed to the kernel
#else
typedef struct u2 {
	UINT Magic;
	Framebuffer* bootframebuffer;
	PSF1_FONT* bootfont;
	MemoryMap MemMap;
	RDSP2* RDSP;
} BootParams; // passed to the kernel
#endif