ifeq ($(TARGET_MCU_SERIES), $(filter $(TARGET_MCU_SERIES), MCXN947))
TARGET_MCU_HAS_LPFLEXCOMM = 1
endif

# Generic CPU features

ifeq ($(TARGET_MCU_CORE_HAS_DSP), 1)
TARGET_CFLAGS_CPU += -mcpu=cortex-m33
else
TARGET_CFLAGS_CPU += -mcpu=cortex-m33+nodsp
endif

ifeq ($(TARGET_MCU_CORE_HAS_FPU), 1)
TARGET_CFLAGS_CPU += -mfloat-abi=hard -mfpu=fpv5-sp-d16
else
TARGET_CFLAGS_CPU += -mfloat-abi=soft
endif

TARGET_CFLAGS_CPU += -mthumb

# Common macros
ifeq ($(TARGET_MCU_CORE),) # Single core MCU
TARGET_MCU_VARIANT = $(TARGET_MCU_SERIES)
TARGET_MCU_DEF = CPU_$(TARGET_MCU_PART)
else # Multi core MCU
TARGET_MCU_VARIANT = $(TARGET_MCU_SERIES)_$(TARGET_MCU_CORE)
TARGET_MCU_DEF = CPU_$(TARGET_MCU_PART)_$(TARGET_MCU_CORE)
endif
