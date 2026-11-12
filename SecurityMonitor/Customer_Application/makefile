# Hey Emacs, this is a -*- makefile -*-
#----------------------------------------------------------------------------
#
# Makefile for ChipWhisperer SimpleSerial-AES Program
#
#----------------------------------------------------------------------------
# On command line:
#
# make all = Make software.
#
# make clean = Clean out built project files.
#
# make coff = Convert ELF to AVR COFF.
#
# make extcoff = Convert ELF to AVR Extended COFF.
#
# make program = Download the hex file to the device, using avrdude.
#                Please customize the avrdude settings below first!
#
# make debug = Start either simulavr or avarice as specified for debugging,
#              with avr-gdb or avr-insight as the front end for debugging.
#
# make filename.s = Just compile filename.c into the assembler code only.
#
# make filename.i = Create a preprocessed source file for use in submitting
#                   bug reports to the GCC project.
#
# To rebuild project do "make clean" then "make all".
#----------------------------------------------------------------------------


TARGET = customer
SRC += customer.c

SS_VER = SS_VER_2_1
CRYPTO_TARGET = TINYAES128C

FIRMWAREPATH ?= /mnt/c/Users/bjark/ChipWhisperer/chipwhisperer/firmware/mcu

include $(FIRMWAREPATH)/Makefile.inc

LINKER_SCRIPT := $(abspath ../linkers/customer.ld)

DEFAULT_LINKER := $(HALPATH)/sam4s/flash_sam4s.ld

LDFLAGS := $(subst -T $(DEFAULT_LINKER),-T$(LINKER_SCRIPT),$(LDFLAGS))
