#include <efi.h>
#include <efilib.h>
#include <elf.h>
#include "../../SharedDefs.h"

UINTN strcmp(CHAR8* a, CHAR8* b, UINTN length){
	for (UINTN i = 0; i < length; i++){
		if (*a != *b) return 0;
	}
	return 1;
}

EFI_FILE* LoadFile(EFI_FILE* dir, CHAR16* Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *ST);
#define LDR_OK 0
#define LDR_FAIL 1
#define LDR_UNIMPLMENTED (-1)
#define STATUS_ASSERT(stat, prg) if(EFI_ERROR(stat)) {Print(L"Assert failed in %a:%d!\n", __FILE__, __LINE__);prg}

Framebuffer boot;

MemoryMap InitalizeMemoryMapStruct(EFI_MEMORY_DESCRIPTOR* Map, UINTN MapSize, UINTN DescSize) {
	MemoryMap new = {
		.Map = Map,
		.MapSize = MapSize,
		.MapDescriptorSize = DescSize
	};
	return new;
}

PSF1_FONT* LoadPSF1Font(CHAR16* Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *ST) {
	EFI_FILE* font = LoadFile(NULL, Path, ImageHandle, ST);
	if (font == NULL) {Print(L"font==null\n"); return NULL;}
	PSF1_HEADER* header;
	ST->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1_HEADER), (void**)&header);
	UINTN size = sizeof(PSF1_HEADER);
	font->Read(font, &size, (VOID*)header);

	UINTN glyphBufferSize = header->charsize * 256;
	if (header->mode == 1) glyphBufferSize = header->charsize * 512;
	void* glyphBuffer;
	font->SetPosition(font, sizeof(PSF1_HEADER));
	ST->BootServices->AllocatePool(EfiLoaderData, glyphBufferSize, (VOID**)&glyphBuffer);
	font->Read(font, &glyphBufferSize, glyphBuffer);
	PSF1_FONT* finishedFont;
	ST->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1_FONT), (void**)&finishedFont);
	finishedFont->Header = header;
	finishedFont->GlyphBuffer = glyphBuffer;
	return finishedFont;
}
Framebuffer* InitalizeGOP() {
	EFI_GUID GOPGuid = gEfiGraphicsOutputProtocolGuid;
	EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
	EFI_STATUS status;
	status = BS->LocateProtocol(&GOPGuid, NULL, (VOID**)&gop);
	STATUS_ASSERT(status, return NULL;);
	Print(L"GOP Interface located\n");
	boot.Address = (void*)gop->Mode->FrameBufferBase;
	boot.BufferSize = (ULONGLONG)gop->Mode->FrameBufferSize;
	boot.Width = (UINT)gop->Mode->Info->HorizontalResolution;
	boot.Height = (UINT)gop->Mode->Info->VerticalResolution;
	boot.PixelsPerScanLine = (UINTN)gop->Mode->Info->PixelsPerScanLine;
	return &boot;
}

BootParams InitalizeParams(Framebuffer* framebuffer, PSF1_FONT* font, MemoryMap mm) {
	BootParams new = {
		.bootframebuffer = framebuffer,
		.bootfont = font,
		.MemMap = mm
	};
	return new;
}

#ifdef ENABLE_GOP_TEST
// draw a line
void TestFramebufferPixels(Framebuffer* buf) {
	UINT y = 50;
	UINT BBP = 4;
	for (UINT x = 0; x < buf->Width / 2 * BBP; x++) {
		*(UINT*)(x + (y * buf->PixelsPerScanLine * BBP) + buf->Address) = 0xffffffff;
	}
}
#endif
EFI_FILE* LoadFile(EFI_FILE* dir, CHAR16* Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *ST) {
	EFI_FILE* LoadedFile;

	EFI_LOADED_IMAGE_PROTOCOL* LoadedImage;
	ST->BootServices->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (VOID**)&LoadedImage);
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FATFs;
	ST->BootServices->HandleProtocol(LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (VOID**)&FATFs);
	if (dir == NULL) {
		FATFs->OpenVolume(FATFs, &dir);
	}
	EFI_STATUS st = dir->Open(dir, &LoadedFile, Path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
	if (st != EFI_SUCCESS) {
		Print(L"In source %a:%d: LoadFile: Error. Error Message: %r. exiting now....", __FILE__, __LINE__, st);
		return NULL;
	}
	return LoadedFile;
}


EFI_STATUS ExecuteKernel(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable, BootParams params) {

	return EFI_SUCCESS;
}
EFI_STATUS efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
	InitializeLib(ImageHandle, SystemTable);
	Print(L"IN UEFI BOOT. Stage 1 of loading the system\n");
	//saved_RT = SystemTable->RuntimeServices;
	Framebuffer* new = InitalizeGOP();
	PSF1_FONT* font = LoadPSF1Font(L"SYSTEM\\FONTS\\early.psf", ImageHandle, SystemTable);
	Print(L"Screen: %dx%d\n", new->Width, new->Height);
	#ifdef ENABLE_GOP_TEST
	TestFramebufferPixels(new);
	#endif
		EFI_FILE* Krnl = LoadFile(NULL, L"kernel.elf", ImageHandle, SystemTable);
	if (Krnl == NULL) {
		Print(L"PANIC: Could not load kernel file successfully!");
		return EFI_NOT_FOUND;
	} else {
		Print(L"Kernel Loaded Successfully\n");
	}
	Elf64_Ehdr Header;
	UINT64 FileInfoSize;
	EFI_FILE_INFO* FileInfo;
	Krnl->GetInfo(Krnl, &gEfiFileInfoGuid, &FileInfoSize, (VOID**)&FileInfo);
	SystemTable->BootServices->AllocatePool(EfiLoaderData, FileInfoSize, (VOID**)&FileInfo);
	Krnl->GetInfo(Krnl, &gEfiFileInfoGuid, &FileInfoSize, (VOID**)&FileInfo);

	UINT64 Size = sizeof(Elf64_Ehdr);
	Krnl->Read(Krnl, &Size, &Header);

	Elf64_Phdr* Phdrs;
	Krnl->SetPosition(Krnl, Header.e_phoff);
	UINTN size = Header.e_phnum * Header.e_phentsize;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, size, (void**)&Phdrs);
	Krnl->Read(Krnl, &size, Phdrs);
	for (
		Elf64_Phdr* phdr = Phdrs;
		(char*)phdr < (char*)Phdrs + Header.e_phnum * Header.e_phentsize;
		phdr = (Elf64_Phdr*)((char*)phdr + Header.e_phentsize)
	)
	{
		switch (phdr->p_type){
			case PT_LOAD:
			{
				int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
				Elf64_Addr segment = phdr->p_paddr;
				SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);

				Krnl->SetPosition(Krnl, phdr->p_offset);
				UINTN size = phdr->p_filesz;
				Krnl->Read(Krnl, &size, (void*)segment);
				break;
			}
		}
	}
	Print(L"Kernel loaded into a state where we can now enter the kernel mode!\n");
	Print(L"Calling into kernel entry!\n");
	void (*KernelEntry)(BootParams) = ((__attribute__((sysv_abi)) void (*)() ) Header.e_entry);
	EFI_MEMORY_DESCRIPTOR* Map = NULL;
	UINTN MapSize, MapKey;
	UINTN DescriptorSize;
	UINT32 DescriptorVersion;
	Print(L"GetMemoryMap1\n");
	SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
	Print(L"AllocatePool\n");
	SystemTable->BootServices->AllocatePool(EfiLoaderData, MapSize, (void**)&Map);
	Print(L"GetMemoryMap2\n");	
	SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
	MemoryMap populatemm = InitalizeMemoryMapStruct(Map, MapSize, DescriptorSize);
	BootParams parameters = InitalizeParams(new, font, populatemm);
	parameters.Magic = BOOTPARAM_MAGIC;
	EFI_CONFIGURATION_TABLE* ConfigTable = SystemTable->ConfigurationTable;
	void* RDSP = NULL;
	EFI_GUID Acpi2TableGuid = ACPI_20_TABLE_GUID;
	for (UINTN i = 0; i < SystemTable->NumberOfTableEntries; ++i) {
		if (CompareGuid(&ConfigTable[i].VendorGuid, &Acpi2TableGuid)) {
			if (strcmp((CHAR8*)"RSD PTR ", (CHAR8*)ConfigTable->VendorTable, 8)) {
				RDSP = (void*)ConfigTable->VendorTable;
			}
		}
		ConfigTable++;
	}
	parameters.RDSP = (RDSP2*)RDSP;
	//SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
	SystemTable->BootServices->ExitBootServices(ImageHandle, MapKey);
	//EFI_STATUS s = ExecuteKernel(ImageHandle, SystemTable, parameters);
	//Print(L"Status translated to message from executing kernel: %r. Exiting EFI Program.....\n", s);
	KernelEntry(parameters);
	// we should never ever get here
	return EFI_ABORTED;
}
