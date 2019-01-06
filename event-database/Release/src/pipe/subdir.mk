################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/pipe/pipe.c 

OBJS += \
./src/pipe/pipe.o 

C_DEPS += \
./src/pipe/pipe.d 


# Each subdirectory must supply rules for building sources it contributes
src/pipe/%.o: ../src/pipe/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I"/home/tommaso/eclipse-workspace/event-database-sdk" -I"/home/tommaso/eclipse-workspace/jansson/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


