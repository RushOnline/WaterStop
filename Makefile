MCU			=	attiny13
MCU_VARIANT	=	attiny13a
MCU_PARTNO	=	t13

CXX 		=	avr-g++
CPPFLAGS	=	-DF_CPU=1200000UL
CXXFLAGS	+=	-Wall -g -Os -mmcu=$(MCU)

program_NAME := WaterStop

program_HEX := $(program_NAME).hex
program_C_SRCS := $(wildcard *.c)
program_CXX_SRCS := $(wildcard *.cpp)
program_C_OBJS := ${program_C_SRCS:.c=.o}
program_CXX_OBJS := ${program_CXX_SRCS:.cpp=.o}
program_OBJS := $(program_C_OBJS) $(program_CXX_OBJS)
program_INCLUDE_DIRS := 
program_LIBRARY_DIRS :=
program_LIBRARIES :=

CPPFLAGS += $(foreach includedir,$(program_INCLUDE_DIRS),-I$(includedir))
LDFLAGS += $(foreach librarydir,$(program_LIBRARY_DIRS),-L$(librarydir))
LDFLAGS += $(foreach library,$(program_LIBRARIES),-l$(library))

.PHONY: all hex flash clean distclean

all: $(program_NAME)

hex: all
	avr-objcopy -O ihex $(program_NAME) $(program_HEX)
	avr-size --format=avr --mcu=$(MCU_VARIANT) $(program_NAME)

flash: hex
	avrdude -p $(MCU_PARTNO) -b 19200 -c avrisp -e -U flash:w:WaterStop.hex

$(program_NAME): $(program_OBJS)
	$(LINK.cc) $(program_OBJS) -o $(program_NAME)

clean:
	@- $(RM) $(program_NAME)
	@- $(RM) $(program_NAME).hex
	@- $(RM) $(program_OBJS)
	@- rm -rf circuit/*.pdsbak circuit/*.workspace
	@- rm -rf Debug Release 

distclean: clean
