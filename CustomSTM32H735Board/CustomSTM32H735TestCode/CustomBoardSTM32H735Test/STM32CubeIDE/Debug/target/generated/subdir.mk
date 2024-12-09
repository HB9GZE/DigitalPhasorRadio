################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../target/generated/OSWrappers.cpp \
../target/generated/STM32DMA.cpp \
../target/generated/SoftwareMJPEGDecoder.cpp \
../target/generated/TouchGFXConfiguration.cpp \
../target/generated/TouchGFXGeneratedHAL.cpp 

OBJS += \
./target/generated/OSWrappers.o \
./target/generated/STM32DMA.o \
./target/generated/SoftwareMJPEGDecoder.o \
./target/generated/TouchGFXConfiguration.o \
./target/generated/TouchGFXGeneratedHAL.o 

CPP_DEPS += \
./target/generated/OSWrappers.d \
./target/generated/STM32DMA.d \
./target/generated/SoftwareMJPEGDecoder.d \
./target/generated/TouchGFXConfiguration.d \
./target/generated/TouchGFXGeneratedHAL.d 


# Each subdirectory must supply rules for building sources it contributes
target/generated/%.o target/generated/%.su target/generated/%.cyclo: ../target/generated/%.cpp target/generated/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DUSE_HAL_DRIVER -DUSE_BPP=24 -DDEBUG -DSTM32H735xx -c -I../../Core/Inc -I../../TouchGFX/App -I../../TouchGFX/target/generated -I../../TouchGFX/target -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I../../Drivers/BSP/Components/Common -I../../Drivers/BSP/STM32H735G-DK -I../../Middlewares/ST/touchgfx/framework/include -I../../TouchGFX/generated/fonts/include -I../../TouchGFX/generated/gui_generated/include -I../../TouchGFX/generated/images/include -I../../TouchGFX/generated/texts/include -I../../TouchGFX/gui/include -I../../TouchGFX/generated/videos/include -I../../LIBJPEG/App -I../../LIBJPEG/Target -I../../Middlewares/Third_Party/LibJPEG/include -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -femit-class-debug-always -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-target-2f-generated

clean-target-2f-generated:
	-$(RM) ./target/generated/OSWrappers.cyclo ./target/generated/OSWrappers.d ./target/generated/OSWrappers.o ./target/generated/OSWrappers.su ./target/generated/STM32DMA.cyclo ./target/generated/STM32DMA.d ./target/generated/STM32DMA.o ./target/generated/STM32DMA.su ./target/generated/SoftwareMJPEGDecoder.cyclo ./target/generated/SoftwareMJPEGDecoder.d ./target/generated/SoftwareMJPEGDecoder.o ./target/generated/SoftwareMJPEGDecoder.su ./target/generated/TouchGFXConfiguration.cyclo ./target/generated/TouchGFXConfiguration.d ./target/generated/TouchGFXConfiguration.o ./target/generated/TouchGFXConfiguration.su ./target/generated/TouchGFXGeneratedHAL.cyclo ./target/generated/TouchGFXGeneratedHAL.d ./target/generated/TouchGFXGeneratedHAL.o ./target/generated/TouchGFXGeneratedHAL.su

.PHONY: clean-target-2f-generated

