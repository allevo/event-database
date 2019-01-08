################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/event-engine.c \
../src/event-net.c \
../src/event-parser.c \
../src/event-storage.c 

O_SRCS += \
../src/event-database.o \
../src/event-engine.o \
../src/event-storage.o 

OBJS += \
./src/event-engine.o \
./src/event-net.o \
./src/event-parser.o \
./src/event-storage.o 

C_DEPS += \
./src/event-engine.d \
./src/event-net.d \
./src/event-parser.d \
./src/event-storage.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I/home/tommaso/Downloads/jansson-2.12/src -I"/home/tommaso/eclipse-workspace/event-database-sdk" -I"/home/tommaso/eclipse-workspace/jansson/src" -O0 -g3 -Wall -Wextra -Werror -Wconversion -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


