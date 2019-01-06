################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/tests/tests.c 

OBJS += \
./src/tests/tests.o 

C_DEPS += \
./src/tests/tests.d 


# Each subdirectory must supply rules for building sources it contributes
src/tests/%.o: ../src/tests/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I/home/tommaso/Downloads/jansson-2.12/src -I"/home/tommaso/eclipse-workspace/event-database-sdk" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


