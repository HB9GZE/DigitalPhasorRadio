################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Segger/SEGGER/SEGGER_RTT.c \
../Segger/SEGGER/SEGGER_RTT_printf.c \
../Segger/SEGGER/SEGGER_SYSVIEW.c 

C_DEPS += \
./Segger/SEGGER/SEGGER_RTT.d \
./Segger/SEGGER/SEGGER_RTT_printf.d \
./Segger/SEGGER/SEGGER_SYSVIEW.d 

OBJS += \
./Segger/SEGGER/SEGGER_RTT.o \
./Segger/SEGGER/SEGGER_RTT_printf.o \
./Segger/SEGGER/SEGGER_SYSVIEW.o 


# Each subdirectory must supply rules for building sources it contributes
Segger/SEGGER/%.o Segger/SEGGER/%.su Segger/SEGGER/%.cyclo: ../Segger/SEGGER/%.c Segger/SEGGER/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DUSE_BPP=24 -DDEBUG -DSTM32H735xx -c -I../../SI5351 -I../../Core/Inc -I../../TouchGFX/App -I../../TouchGFX/target/generated -I../../TouchGFX/target -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I../../Drivers/BSP/Components/Common -I../../Drivers/BSP/STM32H735G-DK -I../../Middlewares/ST/touchgfx/framework/include -I../../TouchGFX/generated/fonts/include -I../../TouchGFX/generated/gui_generated/include -I../../TouchGFX/generated/images/include -I../../TouchGFX/generated/texts/include -I../../TouchGFX/gui/include -I../../TouchGFX/generated/videos/include -I"E:/OneDrive/Projects/PhasorRadio/Code/PhasorRadio1/STM32CubeIDE/SI5351" -I"E:/OneDrive/Projects/PhasorRadio/Code" -I"E:/OneDrive/Projects/PhasorRadio/Code/PhasorRadio1/STM32CubeIDE/Segger" -I"E:/OneDrive/Projects/PhasorRadio/Code/PhasorRadio1/STM32CubeIDE/Segger/Config" -I"E:/OneDrive/Projects/PhasorRadio/Code/PhasorRadio1/STM32CubeIDE/Segger/Sample" -I"E:/OneDrive/Projects/PhasorRadio/Code/PhasorRadio1/STM32CubeIDE/Segger/SEGGER" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Segger-2f-SEGGER

clean-Segger-2f-SEGGER:
	-$(RM) ./Segger/SEGGER/SEGGER_RTT.cyclo ./Segger/SEGGER/SEGGER_RTT.d ./Segger/SEGGER/SEGGER_RTT.o ./Segger/SEGGER/SEGGER_RTT.su ./Segger/SEGGER/SEGGER_RTT_printf.cyclo ./Segger/SEGGER/SEGGER_RTT_printf.d ./Segger/SEGGER/SEGGER_RTT_printf.o ./Segger/SEGGER/SEGGER_RTT_printf.su ./Segger/SEGGER/SEGGER_SYSVIEW.cyclo ./Segger/SEGGER/SEGGER_SYSVIEW.d ./Segger/SEGGER/SEGGER_SYSVIEW.o ./Segger/SEGGER/SEGGER_SYSVIEW.su

.PHONY: clean-Segger-2f-SEGGER

