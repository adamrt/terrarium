# Nuke built-in rules.
.SUFFIXES:

TARGET := kterrarium
IMAGE := image.iso
BUILD := build
TOOLCHAIN := x86_64-elf
CC := $(TOOLCHAIN)-gcc
LD := $(TOOLCHAIN)-ld
CFLAGS := \
	-g \
	-O2 \
	-pipe \
	-Wall \
	-Wextra \
	-std=gnu11 \
	-ffreestanding \
	-fno-stack-protector \
	-fno-stack-check \
	-fno-lto \
	-fno-PIC \
	-ffunction-sections \
	-fdata-sections \
	-m64 \
	-march=x86-64 \
	-mabi=sysv \
	-mno-80387 \
	-mno-mmx \
	-mno-sse \
	-mno-sse2 \
	-mno-red-zone \
	-mcmodel=kernel

CPPFLAGS := \
	-I src \
	$(CPPFLAGS) \
	-MMD \
	-MP

NASMFLAGS := \
	-g \
	-f elf64 \
	$(patsubst -g,-g -F dwarf,$(NASMFLAGS)) \
	-Wall

LDFLAGS := \
	-m elf_x86_64 \
	-nostdlib \
	-static \
	-z max-page-size=0x1000 \
	--gc-sections \
	-T linker.lds

# Use "find" to glob all *.c, *.S, and *.asm files in the tree and obtain the
# object and header dependency file names.
SRCFILES := $(shell find -L src -type f 2>/dev/null | LC_ALL=C sort)
CFILES := $(filter %.c,$(SRCFILES))
ASFILES := $(filter %.S,$(SRCFILES))
NASMFILES := $(filter %.asm,$(SRCFILES))
OBJ := $(addprefix $(BUILD)/obj/,$(CFILES:.c=.c.o) $(ASFILES:.S=.S.o) $(NASMFILES:.asm=.asm.o))
HEADER_DEPS := $(addprefix $(BUILD)/obj/,$(CFILES:.c=.c.d) $(ASFILES:.S=.S.d))

# Default target. This must come first, before header dependencies.
.PHONY: all
all: $(BUILD)/$(IMAGE)

.PHONY: run
run: $(BUILD)/$(IMAGE)
	qemu-system-x86_64 -cdrom $(BUILD)/$(IMAGE)

.PHONY: clean
clean:
	rm -rf build/

-include $(HEADER_DEPS)

# Link
$(BUILD)/bin/$(TARGET): linker.lds $(OBJ)
	mkdir -p "$(dir $@)"
	$(LD) $(LDFLAGS) $(OBJ) -o $@

# Compile C
$(BUILD)/obj/%.c.o: %.c
	mkdir -p "$(dir $@)"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Compile Assembly GCC (Is this true)
$(BUILD)/obj/%.S.o: %.S
	mkdir -p "$(dir $@)"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Compile Assembly NASM
$(BUILD)/obj/%.asm.o: %.asm
	mkdir -p "$(dir $@)"
	nasm $(NASMFLAGS) $< -o $@

# Build ISO image
$(BUILD)/$(IMAGE): $(BUILD)/bin/$(TARGET)
	mkdir -p $(BUILD)
    # Download and build the latest Limine release.
	curl -L https://github.com/Limine-Bootloader/Limine/releases/latest/download/limine-binary.tar.gz | gunzip | tar -x -C $(BUILD)/ -f -
	make -C $(BUILD)/limine-binary

    # Create and copy the relevant files over our iso root
	mkdir -p $(BUILD)/iso_root
	mkdir -p $(BUILD)/iso_root/boot
	cp -v $(BUILD)/bin/$(TARGET) $(BUILD)/iso_root/boot/
	mkdir -p $(BUILD)/iso_root/boot/limine
	cp -v limine.conf $(BUILD)/limine-binary/limine-bios.sys $(BUILD)/limine-binary/limine-bios-cd.bin \
		  $(BUILD)/limine-binary/limine-uefi-cd.bin $(BUILD)/iso_root/boot/limine/

    # Create the EFI boot tree and copy Limine's EFI executables over.
	mkdir -p $(BUILD)/iso_root/EFI/BOOT
	cp -v $(BUILD)/limine-binary/BOOTX64.EFI $(BUILD)/iso_root/EFI/BOOT/
	cp -v $(BUILD)/limine-binary/BOOTIA32.EFI $(BUILD)/iso_root/EFI/BOOT/

    # Create the bootable ISO.
	xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin \
			-no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
			-apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin \
			-efi-boot-part --efi-boot-image --protective-msdos-label \
			$(BUILD)/iso_root -o $(BUILD)/$(IMAGE)

    # Install Limine stage 1 and 2 for legacy BIOS boot.
	./$(BUILD)/limine-binary/limine bios-install $(BUILD)/$(IMAGE)
