# set default flashing serial port, dont override if passed in as an argument
PORT ?= /dev/ttyACM0

# The Charliewatch uses the CC430 5-variant with no LCD controller
MCU ?= cc430f5137

# Currently we're flashing with a launchpad board
PROGRAMMER ?= rf2500

#Standard Debian gcc-msp430 and msp430mcu packages.
CC = msp430-gcc
CFLAGS += \
	-mmcu=$(MCU) \
	-Wall \
	-I. \
	-Os \
	-MMD \
	-MF .$(notdir $@).d \

LDFLAGS += \
	-mmcu=$(MCU) \

OBJS += main.o

all: charliewatch.hex

charliewatch.elf: $(OBJS)
	$(CC) $(LDFLAGS) -T msp430.x -o $@ $^

%.hex: %.elf
	msp430-objcopy -O ihex $< $@

clean:
	rm -rf *.hex *.elf *.o .*.d 

erase:
	$(BSL) -e

flash: charliewatch.elf
	mspdebug \
		$(PROGRAMMER) \
		"prog $<" \
		"run"

dmesg:
	$(BSL) -P goodwatch.hex -uD
sbwdmesg:
	mspdebug tilib "save_raw 0x2400 2048 dmesg.bin"
	strings dmesg.bin

#Same as dmesg, but it gives the target some time to boot first.
run:
	sleep 5
	../bin/cc430-bsl.py -p $(PORT) -P goodwatch.hex -uD

codeplug.hex: codeplug.txt
	../bin/goodwatch-txt2cp.py -i codeplug.txt -o codeplug.hex
flashcp: codeplug.hex
	$(BSL) -Ef codeplug.hex

dump:
	$(BSL) -ed



energytrace.txt: goodwatch.hex sbwflash
	energytrace 60 > energytrace.txt #This will take a minute.
	gnuplot energytrace.gnuplot
	gnuplot energytrace-txt.gnuplot
	../bin/batterylife.py <energytrace.txt

