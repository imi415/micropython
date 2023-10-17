# Determine which board we are compiling for...
ifdef BOARD_DIR
BOARD ?= $(notdir $(BOARD_DIR:/=))
else
BOARD ?= FRDM_MCXN947
BOARD_DIR ?= boards/$(BOARD)
endif

ifeq ($(wildcard $(BOARD_DIR)/.),)
	$(error Invalid BOARD specified: $(BOARD_DIR))
endif

BUILD ?= build-$(BOARD)
GIT_SUBMODULES += lib/nxp_driver

include ../../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

include $(BOARD_DIR)/mpconfigboard.mk

# MicroPython feature configurations
MICROPY_ROM_TEXT_COMPRESSION ?= 1

# include py core make definitions
include $(TOP)/py/py.mk

include mcx.mk

CROSS_COMPILE ?= arm-none-eabi-

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

CFLAGS += $(INC) -Wall -Werror -std=gnu99 -nostdlib $(TARGET_CFLAGS_CPU) $(COPT)

# Configure floating point support
ifeq ($(MICROPY_FLOAT_IMPL),single)
CFLAGS += -DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_FLOAT
CFLAGS += -fsingle-precision-constant
else ifeq ($(MICROPY_FLOAT_IMPL),none)
CFLAGS += -DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_NONE
endif

LDFLAGS += -nostdlib $(addprefix -T,$(TARGET_MCU_LDS)) -Wl,-Map=$(@:.elf=.map) -Wl,--cref -Wl,--gc-sections

CSUPEROPT = -Os # save some code space

# Tune for Debugging or Optimization
CFLAGS += -g  # always include debug info in the ELF
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -DDEBUG
else
CFLAGS += -Os -DNDEBUG
CFLAGS += -fdata-sections -ffunction-sections
endif

# Hardware & SDK C defines
CFLAGS += \
	-DCPU_$(TARGET_MCU_VARIANT) \
	-DMCUXPRESSO_SDK

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
	nearbyintf.c \
	sf_sin.c \
	sf_cos.c \
	)

ifeq ($(SUPPORTS_HARDWARE_FP_SINGLE),1)
SRC_LIBM_C += lib/libm/thumb_vfp_sqrtf.c
else
SRC_LIBM_C += lib/libm/ef_sqrt.c
endif

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
