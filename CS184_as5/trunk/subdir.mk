################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Algebra.cpp \
../NielsDebug.cpp \
../as5.cpp \
../patch.cpp 

OBJS += \
./Algebra.o \
./NielsDebug.o \
./as5.o \
./patch.o 

CPP_DEPS += \
./Algebra.d \
./NielsDebug.d \
./as5.d \
./patch.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/njoubert/workspace/cs184_as5/cml-1_0_0" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


