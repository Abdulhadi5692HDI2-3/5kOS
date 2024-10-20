#include "acpi.h"
#include "../debug.h"
#include "../../../external/printf/printf.h"
namespace ACPI {
    void* FindTable(ACPI::XSDTHeader* Header, char* Signature) {
        int entries = (Header->Length - sizeof(ACPI::XSDTHeader)) / 8;
        for (int t = 0; t < entries; t++) {
            ACPI::XSDTHeader* nxsdt = (ACPI::XSDTHeader*)*(uint64_t*)((uint64_t)Header + sizeof(ACPI::XSDTHeader) + (t * 8));
            for (int i = 0; i < 4; ++i) {
                #ifdef _ACPI_DEBUG
                    KeAssert(nxsdt->Signature[i] == Signature[i]);
                #endif
                if (nxsdt->Signature[i] != Signature[i]) {
                    //printf("line 13 not equals reached!\n");
                    break;
                }
                if (t == 3) {
                    //printf("t == 3!\n");
                    return nxsdt;
                }
            }
        }
        return 0;
    }
}