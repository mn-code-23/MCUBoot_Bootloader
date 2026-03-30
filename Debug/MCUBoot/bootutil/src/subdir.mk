################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MCUBoot/bootutil/src/boot_record.c \
../MCUBoot/bootutil/src/bootutil_area.c \
../MCUBoot/bootutil/src/bootutil_find_key.c \
../MCUBoot/bootutil/src/bootutil_img_hash.c \
../MCUBoot/bootutil/src/bootutil_img_security_cnt.c \
../MCUBoot/bootutil/src/bootutil_loader.c \
../MCUBoot/bootutil/src/bootutil_misc.c \
../MCUBoot/bootutil/src/bootutil_public.c \
../MCUBoot/bootutil/src/caps.c \
../MCUBoot/bootutil/src/ed25519_psa.c \
../MCUBoot/bootutil/src/encrypted.c \
../MCUBoot/bootutil/src/fault_injection_hardening.c \
../MCUBoot/bootutil/src/fault_injection_hardening_delay_rng_mbedtls.c \
../MCUBoot/bootutil/src/image_ecdsa.c \
../MCUBoot/bootutil/src/image_ed25519.c \
../MCUBoot/bootutil/src/image_rsa.c \
../MCUBoot/bootutil/src/image_validate.c \
../MCUBoot/bootutil/src/loader.c \
../MCUBoot/bootutil/src/ram_load.c \
../MCUBoot/bootutil/src/swap_misc.c \
../MCUBoot/bootutil/src/swap_move.c \
../MCUBoot/bootutil/src/swap_offset.c \
../MCUBoot/bootutil/src/swap_scratch.c \
../MCUBoot/bootutil/src/tlv.c 

OBJS += \
./MCUBoot/bootutil/src/boot_record.o \
./MCUBoot/bootutil/src/bootutil_area.o \
./MCUBoot/bootutil/src/bootutil_find_key.o \
./MCUBoot/bootutil/src/bootutil_img_hash.o \
./MCUBoot/bootutil/src/bootutil_img_security_cnt.o \
./MCUBoot/bootutil/src/bootutil_loader.o \
./MCUBoot/bootutil/src/bootutil_misc.o \
./MCUBoot/bootutil/src/bootutil_public.o \
./MCUBoot/bootutil/src/caps.o \
./MCUBoot/bootutil/src/ed25519_psa.o \
./MCUBoot/bootutil/src/encrypted.o \
./MCUBoot/bootutil/src/fault_injection_hardening.o \
./MCUBoot/bootutil/src/fault_injection_hardening_delay_rng_mbedtls.o \
./MCUBoot/bootutil/src/image_ecdsa.o \
./MCUBoot/bootutil/src/image_ed25519.o \
./MCUBoot/bootutil/src/image_rsa.o \
./MCUBoot/bootutil/src/image_validate.o \
./MCUBoot/bootutil/src/loader.o \
./MCUBoot/bootutil/src/ram_load.o \
./MCUBoot/bootutil/src/swap_misc.o \
./MCUBoot/bootutil/src/swap_move.o \
./MCUBoot/bootutil/src/swap_offset.o \
./MCUBoot/bootutil/src/swap_scratch.o \
./MCUBoot/bootutil/src/tlv.o 

C_DEPS += \
./MCUBoot/bootutil/src/boot_record.d \
./MCUBoot/bootutil/src/bootutil_area.d \
./MCUBoot/bootutil/src/bootutil_find_key.d \
./MCUBoot/bootutil/src/bootutil_img_hash.d \
./MCUBoot/bootutil/src/bootutil_img_security_cnt.d \
./MCUBoot/bootutil/src/bootutil_loader.d \
./MCUBoot/bootutil/src/bootutil_misc.d \
./MCUBoot/bootutil/src/bootutil_public.d \
./MCUBoot/bootutil/src/caps.d \
./MCUBoot/bootutil/src/ed25519_psa.d \
./MCUBoot/bootutil/src/encrypted.d \
./MCUBoot/bootutil/src/fault_injection_hardening.d \
./MCUBoot/bootutil/src/fault_injection_hardening_delay_rng_mbedtls.d \
./MCUBoot/bootutil/src/image_ecdsa.d \
./MCUBoot/bootutil/src/image_ed25519.d \
./MCUBoot/bootutil/src/image_rsa.d \
./MCUBoot/bootutil/src/image_validate.d \
./MCUBoot/bootutil/src/loader.d \
./MCUBoot/bootutil/src/ram_load.d \
./MCUBoot/bootutil/src/swap_misc.d \
./MCUBoot/bootutil/src/swap_move.d \
./MCUBoot/bootutil/src/swap_offset.d \
./MCUBoot/bootutil/src/swap_scratch.d \
./MCUBoot/bootutil/src/tlv.d 


# Each subdirectory must supply rules for building sources it contributes
MCUBoot/bootutil/src/%.o MCUBoot/bootutil/src/%.su MCUBoot/bootutil/src/%.cyclo: ../MCUBoot/bootutil/src/%.c MCUBoot/bootutil/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG '-DMBEDTLS_CONFIG_FILE="mbedtls_config.h"' -DUSE_HAL_DRIVER -DSTM32L4R5xx -c -I../Core/Inc -I"E:/STM32/MCUBoot_Bootloader/MCUBoot/bootutil/include" -I"E:/STM32/MCUBoot_Bootloader/MCUBoot/bootutil/src" -I"E:/STM32/MCUBoot_Bootloader/MCUBoot/mcuboot_config" -I"E:/STM32/MCUBoot_Bootloader/MCUBoot/flash_map_backend" -I"E:/STM32/MCUBoot_Bootloader/MCUBoot/crypto/mbedtls/include" -I"E:/STM32/MCUBoot_Bootloader/MCUBoot/crypto/mbedtls/library" -I"E:/STM32/MCUBoot_Bootloader/MCUBoot" -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-MCUBoot-2f-bootutil-2f-src

clean-MCUBoot-2f-bootutil-2f-src:
	-$(RM) ./MCUBoot/bootutil/src/boot_record.cyclo ./MCUBoot/bootutil/src/boot_record.d ./MCUBoot/bootutil/src/boot_record.o ./MCUBoot/bootutil/src/boot_record.su ./MCUBoot/bootutil/src/bootutil_area.cyclo ./MCUBoot/bootutil/src/bootutil_area.d ./MCUBoot/bootutil/src/bootutil_area.o ./MCUBoot/bootutil/src/bootutil_area.su ./MCUBoot/bootutil/src/bootutil_find_key.cyclo ./MCUBoot/bootutil/src/bootutil_find_key.d ./MCUBoot/bootutil/src/bootutil_find_key.o ./MCUBoot/bootutil/src/bootutil_find_key.su ./MCUBoot/bootutil/src/bootutil_img_hash.cyclo ./MCUBoot/bootutil/src/bootutil_img_hash.d ./MCUBoot/bootutil/src/bootutil_img_hash.o ./MCUBoot/bootutil/src/bootutil_img_hash.su ./MCUBoot/bootutil/src/bootutil_img_security_cnt.cyclo ./MCUBoot/bootutil/src/bootutil_img_security_cnt.d ./MCUBoot/bootutil/src/bootutil_img_security_cnt.o ./MCUBoot/bootutil/src/bootutil_img_security_cnt.su ./MCUBoot/bootutil/src/bootutil_loader.cyclo ./MCUBoot/bootutil/src/bootutil_loader.d ./MCUBoot/bootutil/src/bootutil_loader.o ./MCUBoot/bootutil/src/bootutil_loader.su ./MCUBoot/bootutil/src/bootutil_misc.cyclo ./MCUBoot/bootutil/src/bootutil_misc.d ./MCUBoot/bootutil/src/bootutil_misc.o ./MCUBoot/bootutil/src/bootutil_misc.su ./MCUBoot/bootutil/src/bootutil_public.cyclo ./MCUBoot/bootutil/src/bootutil_public.d ./MCUBoot/bootutil/src/bootutil_public.o ./MCUBoot/bootutil/src/bootutil_public.su ./MCUBoot/bootutil/src/caps.cyclo ./MCUBoot/bootutil/src/caps.d ./MCUBoot/bootutil/src/caps.o ./MCUBoot/bootutil/src/caps.su ./MCUBoot/bootutil/src/ed25519_psa.cyclo ./MCUBoot/bootutil/src/ed25519_psa.d ./MCUBoot/bootutil/src/ed25519_psa.o ./MCUBoot/bootutil/src/ed25519_psa.su ./MCUBoot/bootutil/src/encrypted.cyclo ./MCUBoot/bootutil/src/encrypted.d ./MCUBoot/bootutil/src/encrypted.o ./MCUBoot/bootutil/src/encrypted.su ./MCUBoot/bootutil/src/fault_injection_hardening.cyclo ./MCUBoot/bootutil/src/fault_injection_hardening.d ./MCUBoot/bootutil/src/fault_injection_hardening.o ./MCUBoot/bootutil/src/fault_injection_hardening.su ./MCUBoot/bootutil/src/fault_injection_hardening_delay_rng_mbedtls.cyclo ./MCUBoot/bootutil/src/fault_injection_hardening_delay_rng_mbedtls.d ./MCUBoot/bootutil/src/fault_injection_hardening_delay_rng_mbedtls.o ./MCUBoot/bootutil/src/fault_injection_hardening_delay_rng_mbedtls.su ./MCUBoot/bootutil/src/image_ecdsa.cyclo ./MCUBoot/bootutil/src/image_ecdsa.d ./MCUBoot/bootutil/src/image_ecdsa.o ./MCUBoot/bootutil/src/image_ecdsa.su ./MCUBoot/bootutil/src/image_ed25519.cyclo ./MCUBoot/bootutil/src/image_ed25519.d ./MCUBoot/bootutil/src/image_ed25519.o ./MCUBoot/bootutil/src/image_ed25519.su ./MCUBoot/bootutil/src/image_rsa.cyclo ./MCUBoot/bootutil/src/image_rsa.d ./MCUBoot/bootutil/src/image_rsa.o ./MCUBoot/bootutil/src/image_rsa.su ./MCUBoot/bootutil/src/image_validate.cyclo ./MCUBoot/bootutil/src/image_validate.d ./MCUBoot/bootutil/src/image_validate.o ./MCUBoot/bootutil/src/image_validate.su ./MCUBoot/bootutil/src/loader.cyclo ./MCUBoot/bootutil/src/loader.d ./MCUBoot/bootutil/src/loader.o ./MCUBoot/bootutil/src/loader.su ./MCUBoot/bootutil/src/ram_load.cyclo ./MCUBoot/bootutil/src/ram_load.d ./MCUBoot/bootutil/src/ram_load.o ./MCUBoot/bootutil/src/ram_load.su ./MCUBoot/bootutil/src/swap_misc.cyclo ./MCUBoot/bootutil/src/swap_misc.d ./MCUBoot/bootutil/src/swap_misc.o ./MCUBoot/bootutil/src/swap_misc.su ./MCUBoot/bootutil/src/swap_move.cyclo ./MCUBoot/bootutil/src/swap_move.d ./MCUBoot/bootutil/src/swap_move.o ./MCUBoot/bootutil/src/swap_move.su ./MCUBoot/bootutil/src/swap_offset.cyclo ./MCUBoot/bootutil/src/swap_offset.d ./MCUBoot/bootutil/src/swap_offset.o ./MCUBoot/bootutil/src/swap_offset.su ./MCUBoot/bootutil/src/swap_scratch.cyclo ./MCUBoot/bootutil/src/swap_scratch.d ./MCUBoot/bootutil/src/swap_scratch.o ./MCUBoot/bootutil/src/swap_scratch.su ./MCUBoot/bootutil/src/tlv.cyclo ./MCUBoot/bootutil/src/tlv.d ./MCUBoot/bootutil/src/tlv.o ./MCUBoot/bootutil/src/tlv.su

.PHONY: clean-MCUBoot-2f-bootutil-2f-src

