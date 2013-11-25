DIST=dist
BUILD=build
BOOTSRC=src/boot
KRNSRC=src/krn
GCC_OPTS=-Os -m32 -ffreestanding -c -Wall

all:		dist

dist:		bootsect kernel imgmake
	exec $(BUILD)/imgmake $(BUILD)/bootsector.bin $(BUILD)/kernel.bin $(BUILD)/initramfs $(DIST)/floppy.img

bootsect:
	nasm -fbin -o $(BUILD)/bootsector.bin $(BOOTSRC)/bootsector.s

kernel:
	gcc $(GCC_OPTS) -o $(BUILD)/startup.o $(KRNSRC)/startup.c
	gcc $(GCC_OPTS) -o $(BUILD)/kernel.o $(KRNSRC)/kernel.c
	gcc $(GCC_OPTS) -o $(BUILD)/intslib.o $(KRNSRC)/intslib.c
	gcc $(GCC_OPTS) -std=c99 -o $(BUILD)/tty.o $(KRNSRC)/drv/tty.c
	gcc $(GCC_OPTS) -o $(BUILD)/jvm_mm.o $(KRNSRC)/jvm/jmm.c
	gcc $(GCC_OPTS) -std=c99 -o $(BUILD)/kjvm.o $(KRNSRC)/krnjvm.c
	ld -m elf_i386 --oformat binary -Ttext 0x10000 -o $(BUILD)/kernel.bin $(BUILD)/startup.o $(BUILD)/kernel.o $(BUILD)/tty.o $(BUILD)/intslib.o $(BUILD)/kjvm.o $(BUILD)/jvm_mm.o

imgmake:	src/img/make.c
	g++ -o $(BUILD)/imgmake src/img/make.c

clean:
	rm -rf $(DIST)/*
	rm -rf $(BUILD)/*
