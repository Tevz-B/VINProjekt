################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/touch/XPT2046_touch.c 

C_DEPS += \
./Drivers/touch/XPT2046_touch.d 

OBJS += \
./Drivers/touch/XPT2046_touch.o 


# Each subdirectory must supply rules for building sources it contributes
Drivers/touch/%.o Drivers/touch/%.su: ../Drivers/touch/%.c Drivers/touch/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DDEBUG -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../USB_Device/App -I../USB_Device/Target -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/Misko/Inc -I../System/Include -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-touch

clean-Drivers-2f-touch:
	-$(RM) ./Drivers/touch/XPT2046_touch.d ./Drivers/touch/XPT2046_touch.o ./Drivers/touch/XPT2046_touch.su

.PHONY: clean-Drivers-2f-touch

