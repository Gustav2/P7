
# ==========================================
# Project configuration
# ==========================================

PLATFORM ?= CW308_SAM4S

# Windows Python installed with ChipWhisperer
# Replace this with the actual WSL-formatted path
CW_PYTHON ?=/mnt/c/Users/bjark/ChipWhisperer/cwenv/Scripts/python.exe

# ==========================================
# Default target: compile and merge
# ==========================================

.DEFAULT_GOAL := all

.PHONY: all monitor customer combine flash clean

all: combine

# Compile Monitor firmware
monitor:
	$(MAKE) -C Monitor PLATFORM=$(PLATFORM)

# Compile Customer firmware
customer:
	$(MAKE) -C Customer_Application PLATFORM=$(PLATFORM)

# Merge both firmware images
combine: monitor customer
	@echo "Combining firmware images..."
	"$(CW_PYTHON)" "$$(wslpath -w "$(CURDIR)/scripts/CompilerScript.py")"

# Compile, combine and flash to SAM4S
flash: combine
	@echo "Flashing combined firmware..."
	"$(CW_PYTHON)" "$$(wslpath -w "$(CURDIR)/scripts/flash.py")"

# Remove generated files
clean:
	$(MAKE) -C Monitor PLATFORM=$(PLATFORM) clean
	$(MAKE) -C Customer_Application PLATFORM=$(PLATFORM) clean
	rm -f combined.bin
