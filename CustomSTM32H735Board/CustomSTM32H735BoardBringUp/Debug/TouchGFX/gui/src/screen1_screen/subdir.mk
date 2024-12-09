################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../TouchGFX/gui/src/screen1_screen/Screen1Presenter.cpp \
../TouchGFX/gui/src/screen1_screen/Screen1View.cpp 

OBJS += \
./TouchGFX/gui/src/screen1_screen/Screen1Presenter.o \
./TouchGFX/gui/src/screen1_screen/Screen1View.o 

CPP_DEPS += \
./TouchGFX/gui/src/screen1_screen/Screen1Presenter.d \
./TouchGFX/gui/src/screen1_screen/Screen1View.d 


# Each subdirectory must supply rules for building sources it contributes
TouchGFX/gui/src/screen1_screen/%.o TouchGFX/gui/src/screen1_screen/%.su TouchGFX/gui/src/screen1_screen/%.cyclo: ../TouchGFX/gui/src/screen1_screen/%.cpp TouchGFX/gui/src/screen1_screen/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H735xx -c -I../Core/Inc -I../LIBJPEG/App -I../LIBJPEG/Target -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/LibJPEG/include -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/ST/touchgfx/framework/include -I../TouchGFX/generated/fonts/include -I../TouchGFX/generated/gui_generated/include -I../TouchGFX/generated/images/include -I../TouchGFX/generated/texts/include -I../TouchGFX/generated/videos/include -I../TouchGFX/gui/include -I"C:/ProjectsOnC/PhasorRadio/CustomSTM32H735Board/BSP_Files/BSP/Components" -I"C:/ProjectsOnC/PhasorRadio/CustomSTM32H735Board/BSP_Files/BSP/STM32H735G-DK" -I"C:/ProjectsOnC/PhasorRadio/CustomSTM32H735Board/BSP_Files/BSP/Components/ft5336" -I"C:/ProjectsOnC/PhasorRadio/CustomSTM32H735Board/BSP_Files/BSP/Components/s70kl1281" -I../TouchGFX/App -I../TouchGFX/target/generated -I../TouchGFX/target -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TouchGFX-2f-gui-2f-src-2f-screen1_screen

clean-TouchGFX-2f-gui-2f-src-2f-screen1_screen:
	-$(RM) ./TouchGFX/gui/src/screen1_screen/Screen1Presenter.cyclo ./TouchGFX/gui/src/screen1_screen/Screen1Presenter.d ./TouchGFX/gui/src/screen1_screen/Screen1Presenter.o ./TouchGFX/gui/src/screen1_screen/Screen1Presenter.su ./TouchGFX/gui/src/screen1_screen/Screen1View.cyclo ./TouchGFX/gui/src/screen1_screen/Screen1View.d ./TouchGFX/gui/src/screen1_screen/Screen1View.o ./TouchGFX/gui/src/screen1_screen/Screen1View.su

.PHONY: clean-TouchGFX-2f-gui-2f-src-2f-screen1_screen
