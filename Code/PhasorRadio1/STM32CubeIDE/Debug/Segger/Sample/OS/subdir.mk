################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Segger/Sample/OS/SEGGER_SYSVIEW_FreeRTOS.c 

C_DEPS += \
./Segger/Sample/OS/SEGGER_SYSVIEW_FreeRTOS.d 

OBJS += \
./Segger/Sample/OS/SEGGER_SYSVIEW_FreeRTOS.o 


# Each subdirectory must supply rules for building sources it contributes
Segger/Sample/OS/%.o Segger/Sample/OS/%.su Segger/Sample/OS/%.cyclo: ../Segger/Sample/OS/%.c Segger/Sample/OS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DUSE_BPP=24 -DDEBUG -DSTM32H735xx -c -I../../SI5351 -I../../Core/Inc -I../../TouchGFX/App -I../../TouchGFX/target/generated -I../../TouchGFX/target -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I../../Drivers/BSP/Components/Common -I../../Drivers/BSP/STM32H735G-DK -I../../Middlewares/ST/touchgfx/framework/include -I../../TouchGFX/generated/fonts/include -I../../TouchGFX/generated/gui_generated/include -I../../TouchGFX/generated/images/include -I../../TouchGFX/generated/texts/include -I../../TouchGFX/gui/include -I../../TouchGFX/generated/videos/include -I"E:/OneDrive/Projects/PhasorRadio/Code/PhasorRadio1/STM32CubeIDE/SI5351" -I"E:/OneDrive/Projects/PhasorRadio/Code" -I"E:/OneDrive/Projects/PhasorRadio/Code/PhasorRadio1/STM32CubeIDE/Segger" -I"E:/OneDrive/Projects/PhasorRadio/Code/PhasorRadio1/STM32CubeIDE/Segger/Config" -I"E:/OneDrive/Projects/PhasorRadio/Code/PhasorRadio1/STM32CubeIDE/Segger/Sample" -I"E:/OneDrive/Projects/PhasorRadio/Code/PhasorRadio1/STM32CubeIDE/Segger/SEGGER" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Segger-2f-Sample-2f-OS

clean-Segger-2f-Sample-2f-OS:
	-$(RM) ./Segger/Sample/OS/SEGGER_SYSVIEW_FreeRTOS.cyclo ./Segger/Sample/OS/SEGGER_SYSVIEW_FreeRTOS.d ./Segger/Sample/OS/SEGGER_SYSVIEW_FreeRTOS.o ./Segger/Sample/OS/SEGGER_SYSVIEW_FreeRTOS.su

.PHONY: clean-Segger-2f-Sample-2f-OS

