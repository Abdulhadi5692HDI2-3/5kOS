#include "../../SharedDefs.h"
#include "EarlyDisplay.h"
#include "LibKrnl/libkrnl.h"
#include "asm/kasm.h"
#include "serial/serial.h"
#include "../../external/printf/printf.h"
#include "debug.h"
#include "memory/PFAllocator.h"
#include "memory/GeneralMemory.h"
#include "gdt/gdt.h"
#include "interrupt/idt.h"
#include "interrupt/interrupt.h"
#include "acpi/acpi.h"
#include "pci/pci.h"

#define RGB(r, g, b) ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff)

using namespace NSP_EarlyDisplay;
using namespace ACPI;
extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

//using namespace krnlstd;

void _putchar(char character) {
    char array[2] = {character, '\0'};
    DisplayInterface.Print(RGB(255, 255, 255), array);
    DefaultSerialDevice.WriteChar(character);
}

void KePrintMemoryMap(MemoryMap populatemm, NSP_EarlyDisplay::EarlyDisplay Out) {
    ULONGLONG mMapEntries = populatemm.MapSize / populatemm.MapDescriptorSize;
    for (int i = 0; i < mMapEntries; i++) {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((ULONGLONG)populatemm.Map + (i * populatemm.MapDescriptorSize));
        Out.g_curpos.x = 0;
        Out.g_curpos.y = Out.g_curpos.y + 16;

        Out.Print(RGB(107, 28, 217), EFI_MEMORY_TYPE_STRINGS[desc->Type]);
        Out.Print(RGB(0, 0, 0), " ");
        Out.Print(RGB(255, 255, 255), ToString(desc->NumberOfPages * 4096 / 1024));
        Out.Print(RGB(255, 255, 255), " KB");
    }
}

_Idtr Idtr; // the one we're switching to
_Idtr oldIdtr; // the one from uefi
void KeInterruptRegisterEntry(unsigned int entry, uint64_t handler, _Idtr idtr) {
    IdtEntry* ientry = (IdtEntry*)(idtr.Offset + entry * sizeof(IdtEntry));
    ientry->SetOffset(handler);
    ientry->type_attr = IDT_TA_InterruptGate;
    ientry->selector = 0x08;
}
uint64_t KeInterruptGetEntry(unsigned int entry, _Idtr idtr) {
    IdtEntry* ientry = (IdtEntry*)(idtr.Offset + entry * sizeof(IdtEntry));
    return ientry->GetOffset();
}

void KePrepareInterrupts() {
    _sidt(&oldIdtr);
    Idtr.Limit = 0x0FFF;
    Idtr.Offset = (uint64_t)GlobalAllocator.RequestPage();
    // initalize every entry with the undefined handler (we can make changes later before loading the IDT)
    for (int i = 0; i < 256; i++) {
        KeInterruptRegisterEntry(i, (uint64_t)Undefined_Handler, Idtr);
    }
    KeInterruptRegisterEntry(0xE, (uint64_t)PageFault_Handler, Idtr);
    KeInterruptRegisterEntry(0xD, (uint64_t)GPFault_Handler, Idtr);
    KeInterruptRegisterEntry(0x8, (uint64_t)DoubleFault_Handler, Idtr);
    KeInterruptRegisterEntry(0x03, (uint64_t)Breakpoint_Handler, Idtr);
    KeInterruptRegisterEntry(0x21, (uint64_t)KBD_Handler, Idtr);
    //KeInterruptRegisterEntry(0xf8, (uint64_t)Test_Handler, Idtr);
    //KeInterruptRegisterEntry(0x68, (uint64_t)Test_Handler, Idtr);
    RemapPIC();
    _lidt(&Idtr);
    PIC_Disable(); // disable the PIC
    _sti();
}

void KeKernelInitalize(BootParams LoaderParams) {
    //asm ("cli"); // at this stage we dont need interrupts
    SerialDevice COM1;
    COM1.Initalize();
    DefaultSerialDevice = COM1;

    DisplayInterface.Initalize(LoaderParams.bootframebuffer, LoaderParams.bootfont);
    #ifdef _INIT_DEBUG
    printf("KeKernelInitalize: Initalized the framebuffer and display. Framebuffer address: 0x%lx\n", LoaderParams.bootframebuffer->Address);
    #endif

    SetupGDTSegments();
    #ifdef _INIT_DEBUG
    printf("KeKernelInitalize: Setup GDT and segmentation. (useless but required)\n");
    #endif
    PageFrameAllocator PFA;
    uint64_t KernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    uint64_t KernelPages = (uint64_t)KernelSize / 4096 + 1;
    PFA.ReadEFIMemoryMap(LoaderParams.MemMap.Map, LoaderParams.MemMap.MapSize, LoaderParams.MemMap.MapDescriptorSize);
    PFA.LockPages(&_KernelStart, KernelPages);
    GlobalAllocator = PFA;
    #ifdef _INIT_DEBUG
    printf("KeKernelInitalize: Locked %d pages for the kernel!\n", KernelPages);
    #endif
    
    SetupPaging();
    #ifdef _INIT_DEBUG
    printf("KeKernelInitalize: Paging enabled!\n");
    #endif

    KePrepareInterrupts();
    #ifdef _INIT_DEBUG
    printf("KeKernelInitalize: Interrupt descriptor table loaded and interrupt handlers registered!\n");
    #endif
    // clear screen
    #ifdef _INIT_DEBUG
    printf("KeKernelInitalize: Internal kernel init finished! Clearing framebuffer now....\n");
    #endif
    MemSet(LoaderParams.bootframebuffer->Address, 0, LoaderParams.bootframebuffer->BufferSize);
    DisplayInterface.g_curpos = {0, 0};
}

void AcpiInitalize(BootParams LoaderParams) {
    ACPI::XSDTHeader* XSDT = (ACPI::XSDTHeader*)(LoaderParams.RDSP->XsdtAddress);
    printf("XSDT addr: 0x%lx\n", XSDT);
    ACPI::MCFGHeader* MCFG = (ACPI::MCFGHeader*)ACPI::FindTable(XSDT, (char*)"MCFG");
    printf("MCFG addr: 0x%lx\n", MCFG);
    PCI::EnumeratePCI(MCFG);
}

void KeStartup(BootParams LoaderParams) {
    KeKernelInitalize(LoaderParams);
    printf("Hello world!\n");
    printf("Boot params magic: 0x%X\n", LoaderParams.Magic);    
    printf("RSDP Address: 0x%X\n", LoaderParams.RDSP);
    printf("RSDP revision: %d (0 for ACPI 1, 2 for ACPI 2 and above)\n", LoaderParams.RDSP->Revision);
    AcpiInitalize(LoaderParams);
}
extern "C" void _start(BootParams BootParameters) {
    if (BootParameters.Magic != BOOTPARAM_MAGIC) {
        __hcf();
    }
    KeStartup(BootParameters);
    // if we ever get here then just halt
    for (;;) {__hcf();}
}
