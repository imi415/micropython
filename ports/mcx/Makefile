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

# File containing description of content to be frozen into firmware.
FROZEN_MANIFEST ?= boards/manifest.py

# include py core make definitions
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

include mcx.mk

CROSS_COMPILE ?= arm-none-eabi-

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

CFLAGS += $(INC) -Wall -Werror -std=gnu99 -nostdlib $(TARGET_CFLAGS_CPU) $(COPT)

ASFLAGS += $(TARGET_CFLAGS_CPU)

# Configure floating point support
ifeq ($(MICROPY_FLOAT_IMPL),single)
CFLAGS += -DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_FLOAT
CFLAGS += -fsingle-precision-constant
else ifeq ($(MICROPY_FLOAT_IMPL),none)
CFLAGS += -DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_NONE
endif

LDFLAGS += -nostdlib \
	$(addprefix -T,devices/$(TARGET_MCU_SERIES)/$(TARGET_MCU_VARIANT)_flash.ld) \
	-Wl,-Map=$(@:.elf=.map) -Wl,--cref -Wl,--gc-sections

ifneq ($(TARGET_STACK_SIZE),)
LDFLAGS += -Wl,--defsym=__stack_size__=$(TARGET_STACK_SIZE)
endif

ifneq ($(TARGET_HEAP_SIZE),)
LDFLAGS += -Wl,--defsym=__heap_size__=$(TARGET_HEAP_SIZE)
endif

CSUPEROPT = -Os # save some code space

# SDK
SDK_DIR = lib/nxp_driver/sdk
MCUX_DIR = $(SDK_DIR)/devices/$(TARGET_MCU_SERIES)

TARGET_STARTUP_S = devices/$(TARGET_MCU_SERIES)/startup_$(TARGET_MCU_VARIANT)
TARGET_SYSTEM_C = devices/$(TARGET_MCU_SERIES)/system_$(TARGET_MCU_VARIANT)

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
	-D__START=main \
	-D$(TARGET_MCU_DEF) \
	-DMCUXPRESSO_SDK \
	-D__STARTUP_CLEAR_BSS

# Hardware & SDK C includes
CFLAGS += \
	-I$(BOARD_DIR) \
	-I$(TOP)/$(SDK_DIR)/CMSIS/Include \
	-I$(TOP)/$(MCUX_DIR) \
	-I$(TOP)/$(MCUX_DIR)/drivers

# Flags for optional C++ source code
CXXFLAGS += $(filter-out -std=c99,$(CFLAGS))

LIBS = \
	"$(shell $(CC) $(CFLAGS) -print-libgcc-file-name)"

SRC_C = \
	main.c \
	mphalport.c \
	drv_uart.c \
	systick.c \

SRC_BOARD_C = $(addprefix $(BOARD_DIR)/, \
	board.c \
	clock_config.c \
	interrupts.c \
	pin_mux.c \
	)

SRC_SDK_C = $(addprefix $(MCUX_DIR)/,\
	drivers/fsl_clock.c \
	drivers/fsl_common.c \
	drivers/fsl_common_arm.c \
	drivers/fsl_lpi2c.c \
	drivers/fsl_lpspi.c \
	drivers/fsl_lpuart.c \
	drivers/fsl_power.c \
	drivers/fsl_reset.c \
	)

SRC_C += $(TARGET_SYSTEM_C).c
SRC_O += $(TARGET_STARTUP_S).o

ifeq ($(TARGET_MCU_HAS_LPFLEXCOMM), 1)
SRC_SDK_C += $(MCUX_DIR)/drivers/fsl_lpflexcomm.c
CFLAGS += -DMICROPY_HW_HAS_LPFLEXCOMM
endif

ifeq ($(TARGET_MCU_HAS_OSTIMER), 1)
SRC_SDK_C += $(MCUX_DIR)/drivers/fsl_ostimer.c
CFLAGS += -DMICROPY_HW_HAS_OSTIMER
endif

SRC_SHARED_C = $(addprefix shared/,\
	libc/printf.c \
	readline/readline.c \
	runtime/gchelper_native.c \
	runtime/pyexec.c \
	runtime/stdout_helpers.c \
	runtime/sys_stdio_mphal.c \
	)

# LibC sources
SRC_LIBC_C = \
	shared/libc/string0.c

# LibM sources
SRC_LIBM_C = $(addprefix lib/libm/,\
	acoshf.c \
	asinfacosf.c \
	asinhf.c \
	atan2f.c \
	atanf.c \
	atanhf.c \
	ef_rem_pio2.c \
	erf_lgamma.c \
	fmodf.c \
	kf_cos.c \
	kf_rem_pio2.c \
	kf_sin.c \
	kf_tan.c \
	log1pf.c \
	math.c \
	nearbyintf.c \
	roundf.c \
	sf_cos.c \
	sf_erf.c \
	sf_frexp.c \
	sf_ldexp.c \
	sf_modf.c \
	sf_sin.c \
	sf_tan.c \
	wf_lgamma.c \
	wf_tgamma.c \
	)

ifeq ($(SUPPORTS_HARDWARE_FP_SINGLE),1)
SRC_LIBM_C += lib/libm/thumb_vfp_sqrtf.c
else
SRC_LIBM_C += lib/libm/ef_sqrt.c
endif

SRC_O += \
	shared/runtime/gchelper_thumb2.o

SRC_QSTR += $(SRC_C) $(SRC_SHARED_C)

OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_O))
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_BOARD_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_SDK_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_SHARED_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_CXX:.cpp=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_LIBC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_LIBM_C:.c=.o))

all: $(BUILD)/firmware.elf

# Need correct CPU flags to assemble this file.
$(BUILD)/shared/runtime/gchelper_thumb2.o: $(TOP)/shared/runtime/gchelper_thumb2.s
	$(ECHO) "AS $<"
	$(Q)$(AS) $(ASFLAGS) -o $@ $<

# Startup file requires CPP support.
$(BUILD)/$(TARGET_STARTUP_S).o: $(TARGET_STARTUP_S).S
	$(ECHO) "AS $<"
	$(Q)$(CC) $(CFLAGS) -c -xassembler-with-cpp -o $@ $<

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

include $(TOP)/py/mkrules.mk
