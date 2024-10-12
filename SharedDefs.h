#pragma once
// Shared definitions between the kernel and the bootloader
typedef unsigned long long ulonglong;
typedef ulonglong ULONGLONG;
typedef unsigned int UINT;
typedef UINT _uint;
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

// Bootparams struct
typedef struct u2 {
	Framebuffer* bootframebuffer;
	PSF1_FONT* bootfont;
} BootParams; // passed to the kernel
