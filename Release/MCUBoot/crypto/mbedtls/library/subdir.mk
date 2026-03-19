################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MCUBoot/crypto/mbedtls/library/asn1parse.c \
../MCUBoot/crypto/mbedtls/library/asn1write.c \
../MCUBoot/crypto/mbedtls/library/bignum.c \
../MCUBoot/crypto/mbedtls/library/bignum_core.c \
../MCUBoot/crypto/mbedtls/library/bignum_mod.c \
../MCUBoot/crypto/mbedtls/library/bignum_mod_raw.c \
../MCUBoot/crypto/mbedtls/library/constant_time.c \
../MCUBoot/crypto/mbedtls/library/ecdsa.c \
../MCUBoot/crypto/mbedtls/library/ecp.c \
../MCUBoot/crypto/mbedtls/library/ecp_curves.c \
../MCUBoot/crypto/mbedtls/library/memory_buffer_alloc.c \
../MCUBoot/crypto/mbedtls/library/oid.c \
../MCUBoot/crypto/mbedtls/library/pk.c \
../MCUBoot/crypto/mbedtls/library/pk_wrap.c \
../MCUBoot/crypto/mbedtls/library/pkparse.c \
../MCUBoot/crypto/mbedtls/library/pkwrite.c \
../MCUBoot/crypto/mbedtls/library/platform.c \
../MCUBoot/crypto/mbedtls/library/platform_util.c \
../MCUBoot/crypto/mbedtls/library/sha256.c 

OBJS += \
./MCUBoot/crypto/mbedtls/library/asn1parse.o \
./MCUBoot/crypto/mbedtls/library/asn1write.o \
./MCUBoot/crypto/mbedtls/library/bignum.o \
./MCUBoot/crypto/mbedtls/library/bignum_core.o \
./MCUBoot/crypto/mbedtls/library/bignum_mod.o \
./MCUBoot/crypto/mbedtls/library/bignum_mod_raw.o \
./MCUBoot/crypto/mbedtls/library/constant_time.o \
./MCUBoot/crypto/mbedtls/library/ecdsa.o \
./MCUBoot/crypto/mbedtls/library/ecp.o \
./MCUBoot/crypto/mbedtls/library/ecp_curves.o \
./MCUBoot/crypto/mbedtls/library/memory_buffer_alloc.o \
./MCUBoot/crypto/mbedtls/library/oid.o \
./MCUBoot/crypto/mbedtls/library/pk.o \
./MCUBoot/crypto/mbedtls/library/pk_wrap.o \
./MCUBoot/crypto/mbedtls/library/pkparse.o \
./MCUBoot/crypto/mbedtls/library/pkwrite.o \
./MCUBoot/crypto/mbedtls/library/platform.o \
./MCUBoot/crypto/mbedtls/library/platform_util.o \
./MCUBoot/crypto/mbedtls/library/sha256.o 

C_DEPS += \
./MCUBoot/crypto/mbedtls/library/asn1parse.d \
./MCUBoot/crypto/mbedtls/library/asn1write.d \
./MCUBoot/crypto/mbedtls/library/bignum.d \
./MCUBoot/crypto/mbedtls/library/bignum_core.d \
./MCUBoot/crypto/mbedtls/library/bignum_mod.d \
./MCUBoot/crypto/mbedtls/library/bignum_mod_raw.d \
./MCUBoot/crypto/mbedtls/library/constant_time.d \
./MCUBoot/crypto/mbedtls/library/ecdsa.d \
./MCUBoot/crypto/mbedtls/library/ecp.d \
./MCUBoot/crypto/mbedtls/library/ecp_curves.d \
./MCUBoot/crypto/mbedtls/library/memory_buffer_alloc.d \
./MCUBoot/crypto/mbedtls/library/oid.d \
./MCUBoot/crypto/mbedtls/library/pk.d \
./MCUBoot/crypto/mbedtls/library/pk_wrap.d \
./MCUBoot/crypto/mbedtls/library/pkparse.d \
./MCUBoot/crypto/mbedtls/library/pkwrite.d \
./MCUBoot/crypto/mbedtls/library/platform.d \
./MCUBoot/crypto/mbedtls/library/platform_util.d \
./MCUBoot/crypto/mbedtls/library/sha256.d 


# Each subdirectory must supply rules for building sources it contributes
MCUBoot/crypto/mbedtls/library/%.o MCUBoot/crypto/mbedtls/library/%.su MCUBoot/crypto/mbedtls/library/%.cyclo: ../MCUBoot/crypto/mbedtls/library/%.c MCUBoot/crypto/mbedtls/library/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L4R5xx -c -I../Core/Inc -I"E:/STM32/MCUBoot_Bootloader/MCUBoot/bootutil/src" -I"E:/STM32/MCUBoot_Bootloader/MCUBoot/bootutil/include/sysflash" -I"E:/STM32/MCUBoot_Bootloader/MCUBoot/bootutil/include/bootutil" -I"E:/STM32/MCUBoot_Bootloader/MCUBoot/bootutil/include/bootutil/crypto" -I"E:/STM32/MCUBoot_Bootloader/MCUBoot/flash_map_backend" -I"E:/STM32/MCUBoot_Bootloader/MCUBoot/bootutil/include" -I"E:/STM32/MCUBoot_Bootloader/MCUBoot/crypto/mbedtls/include" -I"E:/STM32/MCUBoot_Bootloader/MCUBoot/mcuboot_config" -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-MCUBoot-2f-crypto-2f-mbedtls-2f-library

clean-MCUBoot-2f-crypto-2f-mbedtls-2f-library:
	-$(RM) ./MCUBoot/crypto/mbedtls/library/asn1parse.cyclo ./MCUBoot/crypto/mbedtls/library/asn1parse.d ./MCUBoot/crypto/mbedtls/library/asn1parse.o ./MCUBoot/crypto/mbedtls/library/asn1parse.su ./MCUBoot/crypto/mbedtls/library/asn1write.cyclo ./MCUBoot/crypto/mbedtls/library/asn1write.d ./MCUBoot/crypto/mbedtls/library/asn1write.o ./MCUBoot/crypto/mbedtls/library/asn1write.su ./MCUBoot/crypto/mbedtls/library/bignum.cyclo ./MCUBoot/crypto/mbedtls/library/bignum.d ./MCUBoot/crypto/mbedtls/library/bignum.o ./MCUBoot/crypto/mbedtls/library/bignum.su ./MCUBoot/crypto/mbedtls/library/bignum_core.cyclo ./MCUBoot/crypto/mbedtls/library/bignum_core.d ./MCUBoot/crypto/mbedtls/library/bignum_core.o ./MCUBoot/crypto/mbedtls/library/bignum_core.su ./MCUBoot/crypto/mbedtls/library/bignum_mod.cyclo ./MCUBoot/crypto/mbedtls/library/bignum_mod.d ./MCUBoot/crypto/mbedtls/library/bignum_mod.o ./MCUBoot/crypto/mbedtls/library/bignum_mod.su ./MCUBoot/crypto/mbedtls/library/bignum_mod_raw.cyclo ./MCUBoot/crypto/mbedtls/library/bignum_mod_raw.d ./MCUBoot/crypto/mbedtls/library/bignum_mod_raw.o ./MCUBoot/crypto/mbedtls/library/bignum_mod_raw.su ./MCUBoot/crypto/mbedtls/library/constant_time.cyclo ./MCUBoot/crypto/mbedtls/library/constant_time.d ./MCUBoot/crypto/mbedtls/library/constant_time.o ./MCUBoot/crypto/mbedtls/library/constant_time.su ./MCUBoot/crypto/mbedtls/library/ecdsa.cyclo ./MCUBoot/crypto/mbedtls/library/ecdsa.d ./MCUBoot/crypto/mbedtls/library/ecdsa.o ./MCUBoot/crypto/mbedtls/library/ecdsa.su ./MCUBoot/crypto/mbedtls/library/ecp.cyclo ./MCUBoot/crypto/mbedtls/library/ecp.d ./MCUBoot/crypto/mbedtls/library/ecp.o ./MCUBoot/crypto/mbedtls/library/ecp.su ./MCUBoot/crypto/mbedtls/library/ecp_curves.cyclo ./MCUBoot/crypto/mbedtls/library/ecp_curves.d ./MCUBoot/crypto/mbedtls/library/ecp_curves.o ./MCUBoot/crypto/mbedtls/library/ecp_curves.su ./MCUBoot/crypto/mbedtls/library/memory_buffer_alloc.cyclo ./MCUBoot/crypto/mbedtls/library/memory_buffer_alloc.d ./MCUBoot/crypto/mbedtls/library/memory_buffer_alloc.o ./MCUBoot/crypto/mbedtls/library/memory_buffer_alloc.su ./MCUBoot/crypto/mbedtls/library/oid.cyclo ./MCUBoot/crypto/mbedtls/library/oid.d ./MCUBoot/crypto/mbedtls/library/oid.o ./MCUBoot/crypto/mbedtls/library/oid.su ./MCUBoot/crypto/mbedtls/library/pk.cyclo ./MCUBoot/crypto/mbedtls/library/pk.d ./MCUBoot/crypto/mbedtls/library/pk.o ./MCUBoot/crypto/mbedtls/library/pk.su ./MCUBoot/crypto/mbedtls/library/pk_wrap.cyclo ./MCUBoot/crypto/mbedtls/library/pk_wrap.d ./MCUBoot/crypto/mbedtls/library/pk_wrap.o ./MCUBoot/crypto/mbedtls/library/pk_wrap.su ./MCUBoot/crypto/mbedtls/library/pkparse.cyclo ./MCUBoot/crypto/mbedtls/library/pkparse.d ./MCUBoot/crypto/mbedtls/library/pkparse.o ./MCUBoot/crypto/mbedtls/library/pkparse.su ./MCUBoot/crypto/mbedtls/library/pkwrite.cyclo ./MCUBoot/crypto/mbedtls/library/pkwrite.d ./MCUBoot/crypto/mbedtls/library/pkwrite.o ./MCUBoot/crypto/mbedtls/library/pkwrite.su ./MCUBoot/crypto/mbedtls/library/platform.cyclo ./MCUBoot/crypto/mbedtls/library/platform.d ./MCUBoot/crypto/mbedtls/library/platform.o ./MCUBoot/crypto/mbedtls/library/platform.su ./MCUBoot/crypto/mbedtls/library/platform_util.cyclo ./MCUBoot/crypto/mbedtls/library/platform_util.d ./MCUBoot/crypto/mbedtls/library/platform_util.o ./MCUBoot/crypto/mbedtls/library/platform_util.su ./MCUBoot/crypto/mbedtls/library/sha256.cyclo ./MCUBoot/crypto/mbedtls/library/sha256.d ./MCUBoot/crypto/mbedtls/library/sha256.o ./MCUBoot/crypto/mbedtls/library/sha256.su

.PHONY: clean-MCUBoot-2f-crypto-2f-mbedtls-2f-library

