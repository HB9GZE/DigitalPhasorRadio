################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/Components/mx25lm51245g.c \
../Drivers/BSP/Components/s70kl1281.c 

OBJS += \
./Drivers/BSP/Components/mx25lm51245g.o \
./Drivers/BSP/Components/s70kl1281.o 

C_DEPS += \
./Drivers/BSP/Components/mx25lm51245g.d \
./Drivers/BSP/Components/s70kl1281.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Components/%.o Drivers/BSP/Components/%.su Drivers/BSP/Components/%.cyclo: ../Drivers/BSP/Components/%.c Drivers/BSP/Components/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H735xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"C:/ProjectsOnC/PhasorRadio/CustomSTM32H735Board/BSP_Files/BSP/Components/mx25lm51245g" -I"C:/ProjectsOnC/PhasorRadio/CustomSTM32H735Board/BSP_Files/BSP/STM32H735G-DK" -I"C:/ProjectsOnC/PhasorRadio/CustomSTM32H735Board/BSP_Files/BSP/Components/s70kl1281" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-Components

clean-Drivers-2f-BSP-2f-Components:
	-$(RM) ./Drivers/BSP/Components/mx25lm51245g.cyclo ./Drivers/BSP/Components/mx25lm51245g.d ./Drivers/BSP/Components/mx25lm51245g.o ./Drivers/BSP/Components/mx25lm51245g.su ./Drivers/BSP/Components/s70kl1281.cyclo ./Drivers/BSP/Components/s70kl1281.d ./Drivers/BSP/Components/s70kl1281.o ./Drivers/BSP/Components/s70kl1281.su

.PHONY: clean-Drivers-2f-BSP-2f-Components

