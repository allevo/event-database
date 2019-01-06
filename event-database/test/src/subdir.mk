################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/event-engine.c \
../src/event-storage.c 

OBJS += \
./src/event-engine.o \
./src/event-storage.o 

C_DEPS += \
./src/event-engine.d \
./src/event-storage.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I/home/tommaso/Downloads/jansson-2.12/src -I"/home/tommaso/eclipse-workspace/event-database-sdk" -I"/home/tommaso/eclipse-workspace/jansson/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


