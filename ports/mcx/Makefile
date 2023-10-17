include ../../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# MicroPython feature configurations
MICROPY_ROM_TEXT_COMPRESSION ?= 1

# include py core make definitions
include $(TOP)/py/py.mk

CROSS_COMPILE ?= arm-none-eabi-

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

# !! Use constants for now, will be shifted to board-specific ones.
TARGET_CFLAGS_CPU = -mcpu=cortex-m33 -mthumb -mtune=cortex-m33 -mfloat-abi=hard -mfpu=fpv5-sp-d16
TARGET_LDSCRIPT = MCXN947_cm33_core0_flash.ld

CFLAGS += $(INC) -Wall -Werror -std=gnu99 -nostdlib $(TARGET_CFLAGS_CPU) $(COPT)

# Floating point support
CFLAGS += -DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_FLOAT
CFLAGS += -fsingle-precision-constant

LDFLAGS += -nostdlib -T $(TARGET_LDSCRIPT) -Wl,-Map=$(@:.elf=.map) -Wl,--cref -Wl,--gc-sections

CSUPEROPT = -Os # save some code space

# Tune for Debugging or Optimization
CFLAGS += -g  # always include debug info in the ELF
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -DDEBUG
else
CFLAGS += -Os -DNDEBUG
CFLAGS += -fdata-sections -ffunction-sections
endif

# Flags for optional C++ source code
CXXFLAGS += $(filter-out -std=c99,$(CFLAGS))

LIBS = \
	"$(shell $(CC) $(CFLAGS) -print-libgcc-file-name)"

SRC_C = \
	main.c \
	mphalport.c \
	shared/libc/printf.c \
	shared/readline/readline.c \
	shared/runtime/pyexec.c \
	shared/runtime/stdout_helpers.c \
	$(BUILD)/_frozen_mpy.c \

# LibC sources
SRC_LIBC_C = \
	shared/libc/string0.c

# LibM sources
SRC_LIBM_C = $(addprefix lib/libm/,\
	atanf.c \
	atan2f.c \
	ef_rem_pio2.c \
	fmodf.c \
	kf_rem_pio2.c \
	kf_sin.c \
	kf_cos.c \
	math.c \
	sf_sin.c \
	sf_cos.c \
	)

SRC_LIBM_C += lib/libm/thumb_vfp_sqrtf.c

SRC_QSTR += \
	shared/readline/readline.c \
	shared/runtime/pyexec.c \

OBJ += $(PY_CORE_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_CXX:.cpp=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_LIBC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_LIBM_C:.c=.o))

all: $(BUILD)/firmware.elf

$(BUILD)/_frozen_mpy.c: $(TOP)/tests/frozen/frozentest.mpy $(BUILD)/genhdr/qstrdefs.generated.h
	$(ECHO) "MISC freezing bytecode"
	$(Q)$(TOP)/tools/mpy-tool.py -f -q $(BUILD)/genhdr/qstrdefs.preprocessed.h -mlongint-impl=none $< > $@

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -O binary -j .isr_vector -j .text -j .data $^ $(BUILD)/firmware.bin

include $(TOP)/py/mkrules.mk
