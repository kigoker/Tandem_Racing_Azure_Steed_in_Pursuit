################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../code/Control.c \
../code/Encoder.c \
../code/Init_car.c \
../code/Motor.c \
../code/PID.c \
../code/Wireless_transmission.c \
../code/camera.c \
../code/image_dealwith.c \
../code/imu.c 

COMPILED_SRCS += \
./code/Control.src \
./code/Encoder.src \
./code/Init_car.src \
./code/Motor.src \
./code/PID.src \
./code/Wireless_transmission.src \
./code/camera.src \
./code/image_dealwith.src \
./code/imu.src 

C_DEPS += \
./code/Control.d \
./code/Encoder.d \
./code/Init_car.d \
./code/Motor.d \
./code/PID.d \
./code/Wireless_transmission.d \
./code/camera.d \
./code/image_dealwith.d \
./code/imu.d 

OBJS += \
./code/Control.o \
./code/Encoder.o \
./code/Init_car.o \
./code/Motor.o \
./code/PID.o \
./code/Wireless_transmission.o \
./code/camera.o \
./code/image_dealwith.o \
./code/imu.o 


# Each subdirectory must supply rules for building sources it contributes
code/%.src: ../code/%.c code/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING C/C++ Compiler'
	cctc -cs --dep-file="$(basename $@).d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/4.���ܳ�/twocars/Ganjiang_3/Ganjiang_2/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<" && \
	if [ -f "$(basename $@).d" ]; then sed.exe -r  -e 's/\b(.+\.o)\b/code\/\1/g' -e 's/\\/\//g' -e 's/\/\//\//g' -e 's/"//g' -e 's/([a-zA-Z]:\/)/\L\1/g' -e 's/\d32:/@TARGET_DELIMITER@/g; s/\\\d32/@ESCAPED_SPACE@/g; s/\d32/\\\d32/g; s/@ESCAPED_SPACE@/\\\d32/g; s/@TARGET_DELIMITER@/\d32:/g' "$(basename $@).d" > "$(basename $@).d_sed" && cp "$(basename $@).d_sed" "$(basename $@).d" && rm -f "$(basename $@).d_sed" 2>/dev/null; else echo 'No dependency file to process';fi
	@echo 'Finished building: $<'
	@echo ' '

code/%.o: ./code/%.src code/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING Assembler'
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-code

clean-code:
	-$(RM) ./code/Control.d ./code/Control.o ./code/Control.src ./code/Encoder.d ./code/Encoder.o ./code/Encoder.src ./code/Init_car.d ./code/Init_car.o ./code/Init_car.src ./code/Motor.d ./code/Motor.o ./code/Motor.src ./code/PID.d ./code/PID.o ./code/PID.src ./code/Wireless_transmission.d ./code/Wireless_transmission.o ./code/Wireless_transmission.src ./code/camera.d ./code/camera.o ./code/camera.src ./code/image_dealwith.d ./code/image_dealwith.o ./code/image_dealwith.src ./code/imu.d ./code/imu.o ./code/imu.src

.PHONY: clean-code

