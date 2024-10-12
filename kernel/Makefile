
OSNAME = CustomOS

GNUEFI = ../gnu-efi
OVMFDIR = ../OVMFbin
LDS = krnl.ld 
CC = gcc

CFLAGS = -ffreestanding -fshort-wchar
LDFLAGS = -T $(LDS) -shared -Bsymbolic -nostdlib

SRCDIR := src
OBJDIR := lib
BUILDDIR = bin
BOOTEFI := $(GNUEFI)/x86_64/bootloader/main.efi

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

dtime=$$(date +%s)
SRC = $(call rwildcard,$(SRCDIR),*.c)          
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))
DIRS = $(wildcard $(SRCDIR)/*)
all: start kernel end

start:
	@ echo [BUILD] Starting build for x86_64
	@ echo [BUILD] Build starting on $$(date --date="@$(dtime)")

end:
	@ echo [BUILD] Ended build for x86_64
	@ echo [BUILD] Build completed on $$(date --date="@$$(($$(date +%s)-dtime))")

kernel: $(OBJS) link

$(OBJDIR)/%.o: $(SRCDIR)/%.c
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
	mformat -i $(BUILDDIR)/$(OSNAME).img ::
	mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI
	mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI/BOOT
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(BOOTEFI) ::/EFI/BOOT
	mcopy -i $(BUILDDIR)/$(OSNAME).img startup.nsh ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(BUILDDIR)/krnl.elf ::/kernel.elf

run:
	qemu-system-x86_64 -drive file=$(BUILDDIR)/$(OSNAME).img -m 256M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="$(OVMFDIR)/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="$(OVMFDIR)/OVMF_VARS-pure-efi.fd" -net none

clean:
	@ echo [BUILD] Starting clean [cleaning $(OBJDIR)/*]
	rm -rf $(OBJDIR)/*
	@ echo [BUILD] Ended clean
