################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../src/aeabi_romdiv_patch.s 

C_SRCS += \
../src/TP_i2c.c \
../src/cr_startup_lpc80x.c \
../src/crp.c \
../src/lib_UE441_lcd.c 

OBJS += \
./src/TP_i2c.o \
./src/aeabi_romdiv_patch.o \
./src/cr_startup_lpc80x.o \
./src/crp.o \
./src/lib_UE441_lcd.o 

C_DEPS += \
./src/TP_i2c.d \
./src/cr_startup_lpc80x.d \
./src/crp.d \
./src/lib_UE441_lcd.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M0PLUS -D__USE_ROMDIVIDE -D__LPC80X__ -D__REDLIB__ -I"C:\Users\paulg\Documents\MCUXpressoIDE_11.4.1_6260\workspace\TP5_base\inc" -I"C:\Users\paulg\Downloads\bibliotheques_exemples_NXP_pour_LPC804_corriges\common\inc" -I"C:\Users\paulg\Downloads\bibliotheques_exemples_NXP_pour_LPC804_corriges\utilities_lib\inc" -I"C:\Users\paulg\Downloads\bibliotheques_exemples_NXP_pour_LPC804_corriges\peripherals_lib\inc" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m0 -mthumb -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU Assembler'
	arm-none-eabi-gcc -c -x assembler-with-cpp -DDEBUG -D__CODE_RED -DCORE_M0PLUS -D__USE_ROMDIVIDE -D__LPC80X__ -D__REDLIB__ -I"C:\Users\paulg\Downloads\bibliotheques_exemples_NXP_pour_LPC804_corriges\utilities_lib" -I"C:\Users\paulg\Downloads\bibliotheques_exemples_NXP_pour_LPC804_corriges\peripherals_lib" -I"C:\Users\paulg\Documents\MCUXpressoIDE_11.4.1_6260\workspace\TP5_base\inc" -g3 -mcpu=cortex-m0 -mthumb -specs=redlib.specs -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


