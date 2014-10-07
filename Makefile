CXX = avr-g++
CXXFLAGS += -Wall -g -Os -mmcu=attiny13

program_NAME := WaterStop
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
	avr-objcopy -O ihex $(program_NAME) $(program_NAME).hex

flash: hex
	avrdude -p t13 -b 19200 -c avrisp -e -U flash:w:WaterStop.hex

$(program_NAME): $(program_OBJS)
	$(LINK.cc) $(program_OBJS) -o $(program_NAME)

clean:
	@- $(RM) $(program_NAME)
	@- $(RM) $(program_NAME).hex
	@- $(RM) $(program_OBJS)

distclean: clean
