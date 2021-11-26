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
all: TP5_base.axf

dependents:
	-cd C:/Users/paulg/Downloads/bibliotheques_exemples_NXP_pour_LPC804_corriges/peripherals_lib/flash && $(MAKE) all
	-cd C:/Users/paulg/Downloads/bibliotheques_exemples_NXP_pour_LPC804_corriges/utilities_lib/flash && $(MAKE) all

# Tool invocations
TP5_base.axf: $(OBJS) $(USER_OBJS) C:/Users/paulg/Downloads/bibliotheques_exemples_NXP_pour_LPC804_corriges/peripherals_lib/flash/libperipherals_lib.a C:/Users/paulg/Downloads/bibliotheques_exemples_NXP_pour_LPC804_corriges/utilities_lib/flash/libutilities_lib.a
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\paulg\Downloads\bibliotheques_exemples_NXP_pour_LPC804_corriges\utilities_lib\flash" -L"C:\Users\paulg\Downloads\bibliotheques_exemples_NXP_pour_LPC804_corriges\peripherals_lib\flash" -L"C:\Users\paulg\Downloads\bibliotheques_exemples_NXP_pour_LPC804_corriges\common\inc" -L"C:\Users\paulg\Downloads\bibliotheques_exemples_NXP_pour_LPC804_corriges\utilities_lib\inc" -Xlinker -Map="TP5_base.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -mcpu=cortex-m0 -mthumb -T "test2_LPC804_Debug.ld" -o "TP5_base.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) TP5_base.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "TP5_base.axf"; # arm-none-eabi-objcopy -v -O binary "TP5_base.axf" "TP5_base.bin" ; # checksum -p LPC804 -d "TP5_base.bin";
	-@echo ' '

.PHONY: all clean dependents post-build
C:/Users/paulg/Downloads/bibliotheques_exemples_NXP_pour_LPC804_corriges/peripherals_lib/flash/libperipherals_lib.a:
C:/Users/paulg/Downloads/bibliotheques_exemples_NXP_pour_LPC804_corriges/utilities_lib/flash/libutilities_lib.a:

-include ../makefile.targets
