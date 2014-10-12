MCU			= attiny2313
TARGET		= WaterStop
CC			= avr-gcc
CXX			= avr-g++
CPPFLAGS	= -Wall -g -Os -mmcu=$(MCU)

SRCS	= $(wildcard *.cpp)
HEX		= $(TARGET).hex

OBJS = $(SRCS:.cpp=.o)

V		= @-

# .SUFFIXES: .elf .hex .S

.PHONY: all disasm hex flash clean

$(TARGET): $(OBJS)

disasm: $(TARGET)
	$(V) avr-objdump -d $(TARGET)

hex: $(TARGET)
	$(V) avr-objcopy -O ihex $(TARGET) $(HEX)
	$(V) avr-size --format=avr --mcu=$(MCU) $(TARGET)

flash: hex
	$(V) avrdude -p $(MCU_PARTNO) -b 19200 -c avrisp -e -U flash:w:WaterStop.hex

clean:
	$(V) $(RM) $(OBJS)
	$(V) $(RM) $(TARGET) $(TARGET).hex
	$(V) $(RM) circuit/*.pdsbak circuit/*.workspace
	$(V) $(RM) Debug Release 
