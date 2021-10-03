################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: TP3_timer.axf

# Tool invocations
TP3_timer.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\paulg\Downloads\bibliotheques_exemples_NXP_pour_LPC804_corriges\peripherals_lib\flash" -L"C:\Users\paulg\Downloads\bibliotheques_exemples_NXP_pour_LPC804_corriges\utilities_lib\flash" -Xlinker -Map="TP3_timer.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -mcpu=cortex-m0 -mthumb -T "Multi_Timer_Blinky_flash.ld" -o "TP3_timer.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) TP3_timer.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "TP3_timer.axf"; # arm-none-eabi-objcopy -v -O binary "TP3_timer.axf" "TP3_timer.bin" ; # checksum -p LPC804 -d "TP3_timer.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
