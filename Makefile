GCCPARAMS = -m32 -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = \
	obj/boot.o \
	obj/gdt.o \
	obj/interrupts.o \
	obj/interruptsasm.o \
	obj/keyboard.o \
	obj/mouse.o \
	obj/pit.o \
	obj/port.o \
	obj/io.o \
	obj/time.o \
	obj/vsprintf.o\
	obj/kernel.o

obj/%.o: %.cpp
	mkdir -p $(@D)
	gcc $(GCCPARAMS) -o $@ -c $<

obj/%.o: boot/%.cpp
	mkdir -p $(@D)
	gcc $(GCCPARAMS) -o $@ -c $<

obj/%.o: device/%.cpp
	mkdir -p $(@D)
	gcc $(GCCPARAMS) -o $@ -c $<

obj/%.o: lib/%.cpp
	mkdir -p $(@D)
	gcc $(GCCPARAMS) -o $@ -c $<

obj/%.o: boot/%.s
	mkdir -p $(@D)
	as $(ASPARAMS) -o $@ $<

obj/%.o: device/%.s
	mkdir -p $(@D)
	as $(ASPARAMS) -o $@ $<

Kernel.bin: boot/linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

run: RotchOS.iso clean
	(killall VirtualBox && sleep 1) || true
	/usr/lib/virtualbox/VirtualBoxVM --comment "RotchOS" --startvm "{ecdf5d85-ef79-4b2a-96ee-39136744ba6f}"

RotchOS.iso: Kernel.bin
	rm -rf $@
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp $< iso/boot/
	echo 'set timeout=0'                         > iso/boot/grub/grub.cfg
	echo 'set default=0'                        >> iso/boot/grub/grub.cfg
	echo ''                                     >> iso/boot/grub/grub.cfg
	echo 'menuentry "Rotch Operating System" {' >> iso/boot/grub/grub.cfg
	echo '  multiboot /boot/Kernel.bin'         >> iso/boot/grub/grub.cfg
	echo '  boot'                               >> iso/boot/grub/grub.cfg
	echo '}'                                    >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=$@ iso
	rm -rf iso

install: Kernel.bin
	sudo cp $< /boot/Kernel.bin

clean:
	rm -rf obj Kernel.bin *.o
