# Example Usage:
# 	Compile: make compile FQBN="arduino:avr:uno"
# 	Upload: make upload FQBN="arduino:avr:nano" BOARD_OPT="cpu=atmega328old" PORT="/dev/ttyUSB1"

SRC_DIR = ./neon-drift-lights
BUILD_DIR = ./build

ifndef PORT
	PORT = /dev/ttyACM0
endif

ifndef FQBN
	FQBN = "arduino:avr:micro"
endif

ifndef BOARD_OPT
	BOARD_OPT = ""
endif

.SILENT:

.PHONY: compile upload serial clean

default: compile

compile:
	arduino-cli compile --fqbn $(FQBN) --build-path $(BUILD_DIR) $(SRC_DIR)
	echo "Done."

upload:
	arduino-cli compile --fqbn $(FQBN) --board-options $(BOARD_OPT) \
			--upload --port $(PORT) --build-path $(BUILD_DIR) $(SRC_DIR)
	echo "Uploaded."

serial:
	picocom -b 115200 $(PORT)

clean:
	rm -rf $(BUILD_DIR)
