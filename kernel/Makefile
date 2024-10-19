
OSNAME = CustomOS

GNUEFI = ../gnu-efi
OVMFDIR = ../OVMFbin
LDS = krnl.ld 
CC = gcc
AS = nasm
CFLAGS = -ffreestanding -fshort-wchar -mno-red-zone
LDFLAGS = -T $(LDS) -shared -Bsymbolic -nostdlib -z noexecstack
ASFLAGS = -Xgnu -f elf64
SRCDIR := src
OBJDIR := lib
BUILDDIR = bin
BOOTEFI := $(GNUEFI)/x86_64/bootloader/main.efi

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

dtime=$$(date +%s)
SRC = $(call rwildcard,$(SRCDIR),*.cpp)
ASM = $(call rwildcard,$(SRCDIR),*.asm)          
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%_asm.o, $(ASM))
DIRS = $(wildcard $(SRCDIR)/*)
EXTERNAL1 = $(realpath -f ../external/printf/printf.c)

COLOUR_GREEN=\033[0;32m
COLOUR_RED=\033[0;31m
COLOUR_BLUE=\033[0;34m
COLOUR_END=\033[0m

all: start external kernel end

start:
	@ echo "$(COLOUR_GREEN)[BUILD] Starting build for x86_64$(COLOUR_END)"
	@ echo "$(COLOUR_GREEN)[BUILD] Build starting on $$(date --date="@$(dtime)")$(COLOUR_END)"

end:
	@ echo "$(COLOUR_GREEN)[BUILD] Ended build for x86_64$(COLOUR_END)"
	@ echo "$(COLOUR_GREEN)[BUILD] Build completed on $$(date --date="@$$(($$(date +%s)-dtime))")$(COLOUR_END)"

kernel: $(OBJS) link

$(OBJDIR)/%_asm.o: $(SRCDIR)/%.asm
	@ echo "$(COLOUR_BLUE)[BUILD] Assembling $^ --> $@ $(COLOUR_END)"
	@ mkdir -p $(@D)
	@ $(AS) $(ASFLAGS) $^ -o $@

$(OBJDIR)/interrupt/interrupt.o: $(SRCDIR)/interrupt/interrupt.cpp
	@ echo "$(COLOUR_BLUE)[BUILD] Compiling interrupt handlers in $^ --> $@ $(COLOUR_END)"
	@ mkdir -p $(@D)
	@ $(CC) -mno-red-zone -mgeneral-regs-only -ffreestanding -c $^ -o $@
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@ echo "$(COLOUR_BLUE)[BUILD] Compiling $^ --> $@ $(COLOUR_END)"
	@ mkdir -p $(@D)
	@ $(CC) $(CFLAGS) -c $^ -o $@


external:
	@ echo "$(COLOUR_BLUE)[BUILD] Compiling external dependency $(EXTERNAL1) $(COLOUR_END)"
	@ $(CC) $(CFLAGS) -c $(EXTERNAL1) -o $(OBJDIR)/printf.o

link:
	@ echo "$(COLOUR_BLUE)[BUILD] Linking object files from $(OBJDIR)/ $(COLOUR_END)"
	@ echo "$(COLOUR_BLUE)[BUILD] Using linker file [krnl.ld] $(COLOUR_END)"
	@ $(LD) $(LDFLAGS) -o $(BUILDDIR)/krnl.elf$(OBJS) $(OBJDIR)/printf.o
setup:
	@mkdir $(BUILDDIR)
	@mkdir $(SRCDIR)
	@mkdir $(OBJDIR)

buildimg:
	dd if=/dev/zero of=$(BUILDDIR)/$(OSNAME).img bs=512 count=93750
# download font file
	curl -OJ https://www.zap.org.au/projects/console-fonts-zap/src/zap-light16.psf
# continue with making image
	mformat -i $(BUILDDIR)/$(OSNAME).img ::
	mmd -i $(BUILDDIR)/$(OSNAME).img ::/SYSTEM
	mmd -i $(BUILDDIR)/$(OSNAME).img ::/SYSTEM/FONTS
	mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI
	mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI/BOOT
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(BOOTEFI) ::/EFI/BOOT
	mcopy -i $(BUILDDIR)/$(OSNAME).img startup.nsh ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(BUILDDIR)/krnl.elf ::/kernel.elf
	mcopy -i $(BUILDDIR)/$(OSNAME).img zap-light16.psf ::/SYSTEM/FONTS/early.psf
	rm -rf zap-light16.psf
run:
	@ ./run

clean:
	@ echo "$(COLOUR_GREEN)[BUILD] Starting clean [cleaning $(OBJDIR)/* and debug.log] $(COLOUR_END)"
	@ echo "$(COLOUR_GREEN)[BUILD] Removing: $(COLOUR_RED)$(OBJS)$(COLOUR_END)"
	rm -rf $(OBJDIR)/*
	rm -rf debug.log
	@ echo "$(COLOUR_GREEN)[BUILD] Ended clean $(COLOUR_END)"