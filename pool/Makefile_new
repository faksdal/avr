################################################################################
#
# definitions and flags
#
PORT		= usb-0000
DEVICE		= atmega328p
CPU_FREQ	= 16000000UL
PROGRAMMER	= atmelice_isp
BAUD		= 115200

OPTIMIZE	= s
INCLUDE		= -I. -I./src -I/home/jole/.arduino15/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/avr/include/

CFLAGS		= -Wall -O$(OPTIMIZE)
COMPILE		= avr-g++ $(CFLAGS) -DF_CPU=$(CPU_FREQ)
OUTPUTFILE	= blink

#OBJ			= obj/twi328P.o obj/ssd1306.o obj/avr.o
OBJFILES	= obj/blink.o obj/main.o

OBJDIR		= obj
BINDIR		= bin
################################################################################

default: link upload

link: $(OBJFILES)
	$(COMPILE) -mmcu=$(DEVICE) $(OBJFILES) -o $(BINDIR)/$(OUTPUTFILE).elf
	avr-objcopy -j .text -j .data -O ihex $(BINDIR)/$(OUTPUTFILE).elf $(BINDIR)/$(OUTPUTFILE).hex
	avr-size --format=avr --mcu=$(DEVICE) $(BINDIR)/$(OUTPUTFILE).elf

obj/blink.o: blink.cpp
	$(COMPILE) $(INCLUDE) -mmcu=$(DEVICE) -c -o $(OBJDIR)/blink.o blink.cpp

obj/main.o: main.cpp
	$(COMPILE) $(INCLUDE) -mmcu=$(DEVICE) -c -o $(OBJDIR)/main.o main.cpp


#obj/twi328P.o: src/twi328P/twi328P.cpp src/twi328P/twi328P.h
#	$(AVRGCC) -c src/twi328P/twi328P.cpp -o obj/twi328P.o

#obj/ssd1306.o: src/ssd1306/ssd1306.cpp src/ssd1306/ssd1306.h
#	$(AVRGCC) -c src/ssd1306/ssd1306.cpp -o obj/ssd1306.o

#obj/init.o: src/ssd1306/init.cpp src/ssd1306/ssd1306.h
#	$(AVRGCC) -c src/ssd1306/init.cpp -o obj/init.o
	


#obj/main.o: main.cpp
#	@echo "obj/main.o"
#	$(AVRGCC) -c main.cpp -o obj/main.o

upload:
	avrdude -v -p $(DEVICE) -c $(PROGRAMMER) -P $(PORT) -b $(BAUD) -U flash:w:$(BINDIR)/$(OUTPUTFILE).hex:i
	#avrdude -v -p $(DEVICE) -c $(PROGRAMMER) -b $(BAUD) -U flash:w:$(BINDIR)/$(OUTPUTFILE).hex:i
	
clean:
	rm obj/*.o $(BINDIR)/$(OUTPUTFILE).elf $(BINDIR)/$(OUTPUTFILE).hex
################################################################################	
#
# target: dependencies
#	action


