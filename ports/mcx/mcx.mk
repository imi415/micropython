ifeq ($(TARGET_MCU_VARIANT),$(filter $(TARGET_MCU_VARIANT),MCXN947VDF_cm33_core0))
TARGET_CFLAGS_CPU += -mcpu=cortex-m33 -mfloat-abi=hard -mfpu=fpv5-sp-d16
SUPPORTS_HARDWARE_FP_SINGLE = 1
else ifeq ($(TARGET_MCU_VARIANT),$(filter $(TARGET_MCU_VARIANT),MCXN947_cm33_core1 MCXA153VLH))
TARGET_CFLAGS_CPU += -mcpu=cortex-m33+nodsp -mfloat-abi=soft
endif

TARGET_CFLAGS_CPU += -mthumb