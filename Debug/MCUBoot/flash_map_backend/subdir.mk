################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MCUBoot/flash_map_backend/boot_hal.c \
../MCUBoot/flash_map_backend/flash_map_backend.c \
../MCUBoot/flash_map_backend/keys.c 

OBJS += \
./MCUBoot/flash_map_backend/boot_hal.o \
./MCUBoot/flash_map_backend/flash_map_backend.o \
./MCUBoot/flash_map_backend/keys.o 

C_DEPS += \
./MCUBoot/flash_map_backend/boot_hal.d \
./MCUBoot/flash_map_backend/flash_map_backend.d \
./MCUBoot/flash_map_backend/keys.d 


# Each subdirectory must supply rules for building sources it contributes
MCUBoot/flash_map_backend/%.o MCUBoot/flash_map_backend/%.su MCUBoot/flash_map_backend/%.cyclo: ../MCUBoot/flash_map_backend/%.c MCUBoot/flash_map_backend/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG '-DMBEDTLS_CONFIG_FILE="mbedtls_config.h"' -DUSE_HAL_DRIVER -DSTM32L4R5xx -c -I"E:/STM32/MCUBoot_Bootloader/MCUBoot" -I"E:/STM32/MCUBoot_Bootloader/MCUBoot/bootutil/include/os" -I"E:/STM32/MCUBoot_Bootloader/MCUBoot/flash_map_backend" -I"E:/STM32/MCUBoot_Bootloader/MCUBoot/mcuboot_config" -I"E:/STM32/MCUBoot_Bootloader/MCUBoot/bootutil/include" -I"E:/STM32/MCUBoot_Bootloader/MCUBoot/bootutil/src" -I"E:/STM32/MCUBoot_Bootloader/MCUBoot/crypto/mbedtls/include" -I"E:/STM32/MCUBoot_Bootloader/MCUBoot/crypto/mbedtls/library" -I"E:/STM32/MCUBoot_Bootloader/MCUBoot/crypto/mbedtls/include/mbedtls" -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-MCUBoot-2f-flash_map_backend

clean-MCUBoot-2f-flash_map_backend:
	-$(RM) ./MCUBoot/flash_map_backend/boot_hal.cyclo ./MCUBoot/flash_map_backend/boot_hal.d ./MCUBoot/flash_map_backend/boot_hal.o ./MCUBoot/flash_map_backend/boot_hal.su ./MCUBoot/flash_map_backend/flash_map_backend.cyclo ./MCUBoot/flash_map_backend/flash_map_backend.d ./MCUBoot/flash_map_backend/flash_map_backend.o ./MCUBoot/flash_map_backend/flash_map_backend.su ./MCUBoot/flash_map_backend/keys.cyclo ./MCUBoot/flash_map_backend/keys.d ./MCUBoot/flash_map_backend/keys.o ./MCUBoot/flash_map_backend/keys.su

.PHONY: clean-MCUBoot-2f-flash_map_backend

