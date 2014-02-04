DIST=dist
BUILD=build
BOOTSRC=src/boot
KRNSRC=src/krn
CC=gcc -std=c99 -Os -m32 -ffreestanding -c -Wall -march=i386
LD=ld -m elf_i386 --oformat binary 

all: $(DIST)/floppy.img

$(DIST)/floppy.img: $(BUILD)/bootsector.bin $(BUILD)/kernel.bin kernel_lib/dist/initramfs $(BUILD)/imgmake
	exec $(BUILD)/imgmake $(BUILD)/bootsector.bin $(BUILD)/kernel.bin kernel_lib/dist/initramfs $@

$(BUILD)/bootsector.bin: $(BOOTSRC)/bootsector.s
	nasm -fbin -o $(BUILD)/bootsector.bin $(BOOTSRC)/bootsector.s

$(BUILD)/kernel.bin:
	$(CC) -o $(BUILD)/startup.o $(KRNSRC)/startup.c
	$(CC) -o $(BUILD)/kernel.o $(KRNSRC)/kernel.c
	$(CC) -o $(BUILD)/intslib.o $(KRNSRC)/intslib.c
	$(CC) -o $(BUILD)/tty.o $(KRNSRC)/drv/tty.c
	$(CC) -o $(BUILD)/jvm_mm.o $(KRNSRC)/jvm/jmm.c
	$(CC) -o $(BUILD)/kjvm.o $(KRNSRC)/krnjvm.c
	$(LD) -Ttext 0x10000 -o $@ -Map $(BUILD)/kernel.map $(BUILD)/startup.o $(BUILD)/kernel.o $(BUILD)/tty.o $(BUILD)/intslib.o $(BUILD)/kjvm.o $(BUILD)/jvm_mm.o

$(BUILD)/imgmake: src/img/make.c
	g++ -o $@ $<

clean:
	rm -rf $(DIST)/*
	rm -rf $(BUILD)/*

run: $(DIST)/floppy.img
	qemu-system-i386 -fda $< -boot once=a -no-fd-bootchk
