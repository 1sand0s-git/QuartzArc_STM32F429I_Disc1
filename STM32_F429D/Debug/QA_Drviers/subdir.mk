################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../QA_Drviers/QAD_EXTI.cpp \
../QA_Drviers/QAD_GPIO.cpp 

OBJS += \
./QA_Drviers/QAD_EXTI.o \
./QA_Drviers/QAD_GPIO.o 

CPP_DEPS += \
./QA_Drviers/QAD_EXTI.d \
./QA_Drviers/QAD_GPIO.d 


# Each subdirectory must supply rules for building sources it contributes
QA_Drviers/%.o: ../QA_Drviers/%.cpp QA_Drviers/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx -c -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Core -I../QA_Drivers -I../QA_Drivers/QAD_PeripheralManagers -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

