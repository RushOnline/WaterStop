export ARDUINO_DIR		= $(CURDIR)/../Arduino
export ARDUINO_PORT		= $(shell arduino-uart-dev)
export BOARD_TAG		= pro5v328
export ARDUINO_LIBS		= # Timer
export BOARDS_TXT		= $(ARDUINO_DIR)/hardware/arduino/boards.txt
export RESET_CMD		= stty -F $(ARDUINO_PORT) hupcl
export AVRDUDE			= /usr/bin/avrdude
export MONITOR_BAUDRATE	= 57600

# sudo ln -snf /usr/share/arduino/ard-parse-boards /usr/local/bin/ard-parse-boards

include $(CURDIR)/../Arduino-Makefile/Arduino.mk
