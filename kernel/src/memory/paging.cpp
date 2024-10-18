#include "paging.h"
#include "PFAllocator.h"
#include "GeneralMemory.h"
#include "../asm/kasm.h"
#include "../../../external/printf/printf.h"
__attribute__((aligned(4096)))
PageTable PML4;

// bit flags
#define    PAGE_BIT_P_PRESENT (1<<0)
#define    PAGE_BIT_RW_WRITABLE (1<<1)
#define    PAGE_BIT_US_USER (1<<2)
#define    PAGE_XD_NX (1<<63)
#define    PAGE_BIT_A_ACCESSED (1<<5)
#define    PAGE_BIT_D_DIRTY (1<<6)

// 52-bit address
#define PAGE_ADDRESS_MASK 0x000ffffffffff000

void IdentityMap4Kb(uint64_t logical) {
    int flags = PAGE_BIT_P_PRESENT | PAGE_BIT_RW_WRITABLE | PAGE_BIT_US_USER;
    //printf("\nIdentityMap4Kb(logical=0x%lx) flags = 0x%lx", flags);
    int PML4_IDX = (logical >> 39) & 0x1ff;
    int PDP_IDX = (logical >> 30) & 0x1ff;
    int PD_IDX = (logical >> 21) & 0x1ff;
    int PT_IDX = (logical >> 12) & 0x1ff;
    int P_IDX = logical & 0x1ff;
    if (!(PML4.entries[PML4_IDX] & PAGE_BIT_P_PRESENT)) {
        // lets make a new page
        uint64_t PDPT_Alloc = (uint64_t)GlobalAllocator.RequestPage();
        //printf("\nIdentityMap4Kb(logical=0x%lx): PDPT_Alloc Address=0x%lx PDPT_Alloc Value=0x%lx\n", logical, &PDPT_Alloc, PDPT_Alloc);
        // zero it
        //// MemSetZero((void*)PDPT_Alloc, 4096);
        // update the PML4 so it contains the new PDPT
        PML4.entries[PML4_IDX] = (PDPT_Alloc & PAGE_ADDRESS_MASK) | flags;
        IdentityMap4Kb(PDPT_Alloc);
    }

    PageTable* PDPT = (PageTable*)(PML4.entries[PML4_IDX] & PAGE_ADDRESS_MASK);
    if (!(PDPT->entries[PDP_IDX] & PAGE_BIT_P_PRESENT)) {
        uint64_t PDT_ALLOC = (uint64_t)GlobalAllocator.RequestPage();
        //printf("\nIdentityMap4Kb(logical=0x%lx): PDT_Alloc Address=0x%lx PDT_Alloc Value=0x%lx\n", logical, &PDT_ALLOC, PDT_ALLOC);
        // // MemSetZero((void*)PDT_ALLOC, 4096);
        PDPT->entries[PDP_IDX] = (PDT_ALLOC & PAGE_ADDRESS_MASK) | flags;
        IdentityMap4Kb(PDT_ALLOC);
    }

    PageTable* PDT = (PageTable*)(PDPT->entries[PDP_IDX] & PAGE_ADDRESS_MASK);
    if (!(PDT->entries[PD_IDX] & PAGE_BIT_P_PRESENT)) {
        uint64_t PT_ALLOC = (uint64_t)GlobalAllocator.RequestPage();
        //printf("\nIdentityMap4Kb(logical=0x%lx): PT_Alloc Address=0x%lx PT_Alloc Value=0x%lx\n", logical, &PT_ALLOC, PT_ALLOC);
        // // MemSetZero((void*)PT_ALLOC, 4096);
        PDT->entries[PD_IDX] = (PT_ALLOC & PAGE_ADDRESS_MASK) | flags;
        IdentityMap4Kb(PT_ALLOC);
    }

    PageTable* PT = (PageTable*)(PDT->entries[PD_IDX] & PAGE_ADDRESS_MASK);
    if (!(PT->entries[PT_IDX] & PAGE_BIT_P_PRESENT)) {
        PT->entries[PT_IDX] = (logical & PAGE_ADDRESS_MASK);
    }
}

void SetupPaging() {
    //printf("\n ONE LINE BEFORE zeroing PML4!\n");
    // // MemSetZero((void*)&PML4, 4096);
    //printf("ONE LINE AFTER zeroing PML4. Switching onto for loop!\n");
    for (int i = 0, c = 0; i < GlobalAllocator.GetTotalRAM(); i+=4096, c++) {
        //printf("\ncount: %d: IdentityMap4Kb(logical=%d) executing until condition (i < %lu)...\n", c, i, 128*1024*1024);
        IdentityMap4Kb(i);
    }
    _lpml4(&PML4);
}