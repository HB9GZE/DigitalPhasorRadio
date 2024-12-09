################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../generated/fonts/src/ApplicationFontProvider.cpp \
../generated/fonts/src/CachedFont.cpp \
../generated/fonts/src/FontCache.cpp \
../generated/fonts/src/Font_verdana_10_4bpp_0.cpp \
../generated/fonts/src/Font_verdana_20_4bpp_0.cpp \
../generated/fonts/src/Font_verdana_40_4bpp_0.cpp \
../generated/fonts/src/GeneratedFont.cpp \
../generated/fonts/src/Kerning_verdana_10_4bpp.cpp \
../generated/fonts/src/Kerning_verdana_20_4bpp.cpp \
../generated/fonts/src/Kerning_verdana_40_4bpp.cpp \
../generated/fonts/src/Table_verdana_10_4bpp.cpp \
../generated/fonts/src/Table_verdana_20_4bpp.cpp \
../generated/fonts/src/Table_verdana_40_4bpp.cpp \
../generated/fonts/src/UnmappedDataFont.cpp \
../generated/fonts/src/VectorFontRendererBuffers.cpp 

OBJS += \
./generated/fonts/src/ApplicationFontProvider.o \
./generated/fonts/src/CachedFont.o \
./generated/fonts/src/FontCache.o \
./generated/fonts/src/Font_verdana_10_4bpp_0.o \
./generated/fonts/src/Font_verdana_20_4bpp_0.o \
./generated/fonts/src/Font_verdana_40_4bpp_0.o \
./generated/fonts/src/GeneratedFont.o \
./generated/fonts/src/Kerning_verdana_10_4bpp.o \
./generated/fonts/src/Kerning_verdana_20_4bpp.o \
./generated/fonts/src/Kerning_verdana_40_4bpp.o \
./generated/fonts/src/Table_verdana_10_4bpp.o \
./generated/fonts/src/Table_verdana_20_4bpp.o \
./generated/fonts/src/Table_verdana_40_4bpp.o \
./generated/fonts/src/UnmappedDataFont.o \
./generated/fonts/src/VectorFontRendererBuffers.o 

CPP_DEPS += \
./generated/fonts/src/ApplicationFontProvider.d \
./generated/fonts/src/CachedFont.d \
./generated/fonts/src/FontCache.d \
./generated/fonts/src/Font_verdana_10_4bpp_0.d \
./generated/fonts/src/Font_verdana_20_4bpp_0.d \
./generated/fonts/src/Font_verdana_40_4bpp_0.d \
./generated/fonts/src/GeneratedFont.d \
./generated/fonts/src/Kerning_verdana_10_4bpp.d \
./generated/fonts/src/Kerning_verdana_20_4bpp.d \
./generated/fonts/src/Kerning_verdana_40_4bpp.d \
./generated/fonts/src/Table_verdana_10_4bpp.d \
./generated/fonts/src/Table_verdana_20_4bpp.d \
./generated/fonts/src/Table_verdana_40_4bpp.d \
./generated/fonts/src/UnmappedDataFont.d \
./generated/fonts/src/VectorFontRendererBuffers.d 


# Each subdirectory must supply rules for building sources it contributes
generated/fonts/src/%.o generated/fonts/src/%.su generated/fonts/src/%.cyclo: ../generated/fonts/src/%.cpp generated/fonts/src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DUSE_HAL_DRIVER -DUSE_BPP=24 -DDEBUG -DSTM32H735xx -c -I../../Core/Inc -I../../TouchGFX/App -I../../TouchGFX/target/generated -I../../TouchGFX/target -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I../../Drivers/BSP/Components/Common -I../../Drivers/BSP/STM32H735G-DK -I../../Middlewares/ST/touchgfx/framework/include -I../../TouchGFX/generated/fonts/include -I../../TouchGFX/generated/gui_generated/include -I../../TouchGFX/generated/images/include -I../../TouchGFX/generated/texts/include -I../../TouchGFX/gui/include -I../../TouchGFX/generated/videos/include -I../../LIBJPEG/App -I../../LIBJPEG/Target -I../../Middlewares/Third_Party/LibJPEG/include -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -femit-class-debug-always -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-generated-2f-fonts-2f-src

clean-generated-2f-fonts-2f-src:
	-$(RM) ./generated/fonts/src/ApplicationFontProvider.cyclo ./generated/fonts/src/ApplicationFontProvider.d ./generated/fonts/src/ApplicationFontProvider.o ./generated/fonts/src/ApplicationFontProvider.su ./generated/fonts/src/CachedFont.cyclo ./generated/fonts/src/CachedFont.d ./generated/fonts/src/CachedFont.o ./generated/fonts/src/CachedFont.su ./generated/fonts/src/FontCache.cyclo ./generated/fonts/src/FontCache.d ./generated/fonts/src/FontCache.o ./generated/fonts/src/FontCache.su ./generated/fonts/src/Font_verdana_10_4bpp_0.cyclo ./generated/fonts/src/Font_verdana_10_4bpp_0.d ./generated/fonts/src/Font_verdana_10_4bpp_0.o ./generated/fonts/src/Font_verdana_10_4bpp_0.su ./generated/fonts/src/Font_verdana_20_4bpp_0.cyclo ./generated/fonts/src/Font_verdana_20_4bpp_0.d ./generated/fonts/src/Font_verdana_20_4bpp_0.o ./generated/fonts/src/Font_verdana_20_4bpp_0.su ./generated/fonts/src/Font_verdana_40_4bpp_0.cyclo ./generated/fonts/src/Font_verdana_40_4bpp_0.d ./generated/fonts/src/Font_verdana_40_4bpp_0.o ./generated/fonts/src/Font_verdana_40_4bpp_0.su ./generated/fonts/src/GeneratedFont.cyclo ./generated/fonts/src/GeneratedFont.d ./generated/fonts/src/GeneratedFont.o ./generated/fonts/src/GeneratedFont.su ./generated/fonts/src/Kerning_verdana_10_4bpp.cyclo ./generated/fonts/src/Kerning_verdana_10_4bpp.d ./generated/fonts/src/Kerning_verdana_10_4bpp.o ./generated/fonts/src/Kerning_verdana_10_4bpp.su ./generated/fonts/src/Kerning_verdana_20_4bpp.cyclo ./generated/fonts/src/Kerning_verdana_20_4bpp.d ./generated/fonts/src/Kerning_verdana_20_4bpp.o ./generated/fonts/src/Kerning_verdana_20_4bpp.su ./generated/fonts/src/Kerning_verdana_40_4bpp.cyclo ./generated/fonts/src/Kerning_verdana_40_4bpp.d ./generated/fonts/src/Kerning_verdana_40_4bpp.o ./generated/fonts/src/Kerning_verdana_40_4bpp.su ./generated/fonts/src/Table_verdana_10_4bpp.cyclo ./generated/fonts/src/Table_verdana_10_4bpp.d ./generated/fonts/src/Table_verdana_10_4bpp.o ./generated/fonts/src/Table_verdana_10_4bpp.su ./generated/fonts/src/Table_verdana_20_4bpp.cyclo ./generated/fonts/src/Table_verdana_20_4bpp.d ./generated/fonts/src/Table_verdana_20_4bpp.o ./generated/fonts/src/Table_verdana_20_4bpp.su ./generated/fonts/src/Table_verdana_40_4bpp.cyclo ./generated/fonts/src/Table_verdana_40_4bpp.d ./generated/fonts/src/Table_verdana_40_4bpp.o ./generated/fonts/src/Table_verdana_40_4bpp.su ./generated/fonts/src/UnmappedDataFont.cyclo ./generated/fonts/src/UnmappedDataFont.d ./generated/fonts/src/UnmappedDataFont.o ./generated/fonts/src/UnmappedDataFont.su ./generated/fonts/src/VectorFontRendererBuffers.cyclo ./generated/fonts/src/VectorFontRendererBuffers.d ./generated/fonts/src/VectorFontRendererBuffers.o ./generated/fonts/src/VectorFontRendererBuffers.su

.PHONY: clean-generated-2f-fonts-2f-src

