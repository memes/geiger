# Sparkfun geiger counter properties
MCU := atmega328p
F_CPU := 8000000

# Programmer properties; Pololu USB AVR programmer
PROGRAMMER := avrispv2
PROGRAMMER_DEV := /dev/ttyACM0

# Basename of ouptut files
TGT := geiger

# Add additional includes and libraries here
INCLUDE_DIRS := 
LIBS :=

# Standard make file follows - 
# Override as necessary, but should be fine for most projects
SRC := $(wildcard *.c *.S)
OBJS := $(filter %.o, $(patsubst %.c, %.o, $(SRC)) $(patsubst %.S, %.o, $(SRC)))
DEPS := $(patsubst %.o, %.d, $(OBJS))
CC := avr-gcc
RM := rm -f
AVRGDB := avr-gdb
OBJCOPY := avr-objcopy
AVRDUDE := avrdude
SIMAVR := simavr
GDBINIT := .gdbinit
GDBPORT := 1234
GDBHOST := localhost

# Compiler and linker flags
CFLAGS := -g3 -gdwarf-2 -Os -funsigned-char -funsigned-bitfields -fpack-struct \
	-fshort-enums -Wall -Wstrict-prototypes \
	-DF_CPU=$(F_CPU) \
	-I. $(patsubst %, -I%, $(INCLUDE_DIRS)) \
	-mmcu=$(MCU) -std=gnu99

ASFLAGS := -Wa,-gstabs \
	-I. $(patsubst %, -I%, $(INCLUDE_DIRS)) \
	-DF_CPU=$(F_CPU) \
	-mmcu=$(MCU) -std=gnuc99

LDFLAGS := -Wl,-Map=$(TGT).map,--cref \
	-mmcu=$(MCU) \
	$(patsubst %, -l%, $(LIBS))

# Derived properties:-
#  - if PROGRAMMER_MCU is undefined, attempt to figure it out from avrdude
# Note: shell fragment is a mess (grep and cut) but attempting to use awk to
# combine failed
ifeq ($(strip $(PROGRAMMER_MCU)), )
PROGRAMMER_MCU := $(shell $(AVRDUDE) -p ? 2>&1 | grep -i $(MCU) | cut -d' ' -f3 )
endif

# Handle dependencies
-include $(DEPS)

# Provide a default target
default: $(TGT).hex

# Keep Emacs flymake happy; check-syntax and dumpvars
check-syntax:
	@$(CC) $(CFLAGS) -fsyntax-only \
		 -o /dev/null -S $(CHK_SOURCES)

dumpvars:
	@echo $(foreach var, $(DUMPVARS), "$(var)=$($(var))")

# Real rules begin
clean:
	@$(RM) $(TGT).map $(TGT).elf $(TGT).hex $(GDBINIT)
	@$(RM) $(OBJS)

realclean: clean
	@$(RM) *~

debug: $(GDBINIT) $(TGT).elf
	@$(SIMAVR) -g -m $(MCU) -f $(F_CPU) &
	@$(AVRGDB) --command=$(GDBINIT)
	@pkill $(SIMAVR)

install: $(TGT).hex
ifeq ($(strip $(PROGRAMMER_MCU)),)
	$(error Programmer MCU is undefined)
endif
ifeq ($(strip $(PROGRAMMER_DEV)),)
	$(error Programmer device is undefined)
endif
	$(AVRDUDE) -c $(PROGRAMMER) \
		-p $(PROGRAMMER_MCU) \
		-P $(PROGRAMMER_DEV) \
		-e -U flash:w:$(TGT).hex

$(GDBINIT):
	@$(RM) $@
	@echo define reset >> $@
	@echo SIGNAL SIGHUP >> $@
	@echo end >> $@
	@echo file $(TGT).elf >> $@
	@echo target remote $(GDBHOST):$(GDBPORT) >> $@
	@echo load >> $@
	@echo break main >> $@
	@echo continue >> $@

%.o: %.c
	$(CC) $(CFLAGS) -MMD -c $< -o $@

$(TGT).elf: $(OBJS)
	$(CC) $(LDFLAGS) -o $(TGT).elf $(OBJS)

%.hex: %.elf
	$(OBJCOPY) -R .eeprom -O ihex $< $@

.PHONY: default clean realclean check-syntax dumpvars debug install

