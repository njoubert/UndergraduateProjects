################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Debug.cpp \
../src/Mesh.cpp \
../src/Solver.cpp \
../src/System.cpp \
../src/main.cpp \
../src/testMesh.cpp 

O_SRCS += \
../src/Mesh.o 

OBJS += \
./src/Debug.o \
./src/Mesh.o \
./src/Solver.o \
./src/System.o \
./src/main.o \
./src/testMesh.o 

CPP_DEPS += \
./src/Debug.d \
./src/Mesh.d \
./src/Solver.d \
./src/System.d \
./src/main.d \
./src/testMesh.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/X11/include -O0 -g3 -Wall -c -fmessage-length=0 -L"/System/Library/Frameworks/OpenGL.framework/Libraries" -DOSX -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


