################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/BinaryNum.cpp \
../src/BpNode.cpp \
../src/LDPCDecoder.cpp \
../src/LDPCEncoder.cpp \
../src/QAMDecoder.cpp \
../src/Simulation.cpp \
../src/SymbolNode.cpp 

OBJS += \
./src/BinaryNum.o \
./src/BpNode.o \
./src/LDPCDecoder.o \
./src/LDPCEncoder.o \
./src/QAMDecoder.o \
./src/Simulation.o \
./src/SymbolNode.o 

CPP_DEPS += \
./src/BinaryNum.d \
./src/BpNode.d \
./src/LDPCDecoder.d \
./src/LDPCEncoder.d \
./src/QAMDecoder.d \
./src/Simulation.d \
./src/SymbolNode.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


