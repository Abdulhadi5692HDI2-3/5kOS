#include <efi.h>
#include <efilib.h>
#include <elf.h>
#include "../../SharedDefs.h"

#define LDR_OK 0
#define LDR_FAIL 1
#define LDR_UNIMPLMENTED (-1)
#define STATUS_ASSERT(stat, prg) if(EFI_ERROR(stat)) {Print(L"Assert failed in %a:%d!\n", __FILE__, __LINE__);prg}

Framebuffer boot;
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

BootParams InitalizeParams(Framebuffer* framebuffer) {
	BootParams new = {
		.bootframebuffer = framebuffer
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
	KernelEntry(params);
	return EFI_SUCCESS;
}
EFI_STATUS efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
	InitializeLib(ImageHandle, SystemTable);
	Print(L"IN UEFI BOOT. Stage 1 of loading the system\n");
	Framebuffer* new = InitalizeGOP();
	Print(L"Screen: %dx%d\n", new->Width, new->Height);
	#ifdef ENABLE_GOP_TEST
	TestFramebufferPixels(new);
	#endif
	BootParams parameters = InitalizeParams(new);
	EFI_STATUS s = ExecuteKernel(ImageHandle, SystemTable, parameters);
	Print(L"Status translated to message from executing kernel: %r. Exiting EFI Program.....\n", s);
	return EFI_SUCCESS;
}
