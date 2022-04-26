################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Misko/XPT2046_touch.c \
../Drivers/Misko/ili9341.c \
../Drivers/Misko/ili9341_reg.c \
../Drivers/Misko/lcd_io.c \
../Drivers/Misko/ugui.c 

C_DEPS += \
./Drivers/Misko/XPT2046_touch.d \
./Drivers/Misko/ili9341.d \
./Drivers/Misko/ili9341_reg.d \
./Drivers/Misko/lcd_io.d \
./Drivers/Misko/ugui.d 

OBJS += \
./Drivers/Misko/XPT2046_touch.o \
./Drivers/Misko/ili9341.o \
./Drivers/Misko/ili9341_reg.o \
./Drivers/Misko/lcd_io.o \
./Drivers/Misko/ugui.o 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Misko/%.o Drivers/Misko/%.su: ../Drivers/Misko/%.c Drivers/Misko/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../USB_Device/App -I../USB_Device/Target -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I"../Drivers/Misko/Inc" -I../System/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-Misko

clean-Drivers-2f-Misko:
	-$(RM) ./Drivers/Misko/XPT2046_touch.d ./Drivers/Misko/XPT2046_touch.o ./Drivers/Misko/XPT2046_touch.su ./Drivers/Misko/ili9341.d ./Drivers/Misko/ili9341.o ./Drivers/Misko/ili9341.su ./Drivers/Misko/ili9341_reg.d ./Drivers/Misko/ili9341_reg.o ./Drivers/Misko/ili9341_reg.su ./Drivers/Misko/lcd_io.d ./Drivers/Misko/lcd_io.o ./Drivers/Misko/lcd_io.su ./Drivers/Misko/ugui.d ./Drivers/Misko/ugui.o ./Drivers/Misko/ugui.su

.PHONY: clean-Drivers-2f-Misko

