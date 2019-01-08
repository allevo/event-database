################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/event-database.c \
../src/event-engine.c \
../src/event-net.c \
../src/event-storage.c 

O_SRCS += \
../src/event-database.o \
../src/event-engine.o \
../src/event-storage.o 

OBJS += \
./src/event-database.o \
./src/event-engine.o \
./src/event-net.o \
./src/event-storage.o 

C_DEPS += \
./src/event-database.d \
./src/event-engine.d \
./src/event-net.d \
./src/event-storage.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I"/home/tommaso/eclipse-workspace/event-database-sdk" -I"/home/tommaso/eclipse-workspace/jansson/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


