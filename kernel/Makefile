
OSNAME = CustomOS

GNUEFI = ../gnu-efi
OVMFDIR = ../OVMFbin
LDS = krnl.ld 
CC = gcc
AS = nasm
CFLAGS = -ffreestanding -fshort-wchar
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
all: start kernel end

start:
	@ echo [BUILD] Starting build for x86_64
	@ echo [BUILD] Build starting on $$(date --date="@$(dtime)")

end:
	@ echo [BUILD] Ended build for x86_64
	@ echo [BUILD] Build completed on $$(date --date="@$$(($$(date +%s)-dtime))")

kernel: $(OBJS) link

$(OBJDIR)/%_asm.o: $(SRCDIR)/%.asm
	@ echo [BUILD] Assembling $^ --\> $@
	@ mkdir -p $(@D)
	$(AS) $(ASFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@ echo [BUILD] Compiling $^ --\> $@
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@


link:
	@ echo [BUILD] Linking object files from $(OBJDIR)/
	@ echo [BUILD] Using linker file [krnl.ld]
	$(LD) $(LDFLAGS) -o $(BUILDDIR)/krnl.elf$(OBJS)
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
	qemu-system-x86_64 -drive file=$(BUILDDIR)/$(OSNAME).img -m 256M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="$(OVMFDIR)/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="$(OVMFDIR)/OVMF_VARS-pure-efi.fd" -net none

clean:
	@ echo [BUILD] Starting clean [cleaning $(OBJDIR)/*]
	rm -rf $(OBJDIR)/*
	@ echo [BUILD] Ended clean
