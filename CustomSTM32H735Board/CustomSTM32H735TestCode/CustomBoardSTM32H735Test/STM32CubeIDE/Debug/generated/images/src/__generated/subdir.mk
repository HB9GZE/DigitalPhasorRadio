################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../generated/images/src/__generated/image_alternate_theme_images_widgets_button_regular_height_50_large_rounded_action.cpp \
../generated/images/src/__generated/image_alternate_theme_images_widgets_button_regular_height_50_large_rounded_active.cpp 

OBJS += \
./generated/images/src/__generated/image_alternate_theme_images_widgets_button_regular_height_50_large_rounded_action.o \
./generated/images/src/__generated/image_alternate_theme_images_widgets_button_regular_height_50_large_rounded_active.o 

CPP_DEPS += \
./generated/images/src/__generated/image_alternate_theme_images_widgets_button_regular_height_50_large_rounded_action.d \
./generated/images/src/__generated/image_alternate_theme_images_widgets_button_regular_height_50_large_rounded_active.d 


# Each subdirectory must supply rules for building sources it contributes
generated/images/src/__generated/%.o generated/images/src/__generated/%.su generated/images/src/__generated/%.cyclo: ../generated/images/src/__generated/%.cpp generated/images/src/__generated/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DUSE_HAL_DRIVER -DUSE_BPP=24 -DDEBUG -DSTM32H735xx -c -I../../Core/Inc -I../../TouchGFX/App -I../../TouchGFX/target/generated -I../../TouchGFX/target -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I../../Drivers/BSP/Components/Common -I../../Drivers/BSP/STM32H735G-DK -I../../Middlewares/ST/touchgfx/framework/include -I../../TouchGFX/generated/fonts/include -I../../TouchGFX/generated/gui_generated/include -I../../TouchGFX/generated/images/include -I../../TouchGFX/generated/texts/include -I../../TouchGFX/gui/include -I../../TouchGFX/generated/videos/include -I../../LIBJPEG/App -I../../LIBJPEG/Target -I../../Middlewares/Third_Party/LibJPEG/include -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -femit-class-debug-always -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-generated-2f-images-2f-src-2f-__generated

clean-generated-2f-images-2f-src-2f-__generated:
	-$(RM) ./generated/images/src/__generated/image_alternate_theme_images_widgets_button_regular_height_50_large_rounded_action.cyclo ./generated/images/src/__generated/image_alternate_theme_images_widgets_button_regular_height_50_large_rounded_action.d ./generated/images/src/__generated/image_alternate_theme_images_widgets_button_regular_height_50_large_rounded_action.o ./generated/images/src/__generated/image_alternate_theme_images_widgets_button_regular_height_50_large_rounded_action.su ./generated/images/src/__generated/image_alternate_theme_images_widgets_button_regular_height_50_large_rounded_active.cyclo ./generated/images/src/__generated/image_alternate_theme_images_widgets_button_regular_height_50_large_rounded_active.d ./generated/images/src/__generated/image_alternate_theme_images_widgets_button_regular_height_50_large_rounded_active.o ./generated/images/src/__generated/image_alternate_theme_images_widgets_button_regular_height_50_large_rounded_active.su

.PHONY: clean-generated-2f-images-2f-src-2f-__generated

