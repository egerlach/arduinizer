################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Arduinizer.axf

# Tool invocations
Arduinizer.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Documents and Settings\amackie\My Documents\SVN\CMSISv1p30_LPC13xx\Release" -L"C:\Documents and Settings\amackie\My Documents\SVN\LPC13xx_Lib\Release" -Xlinker --gc-sections -Xlinker -Map=Arduinizer.map -mcpu=cortex-m3 -mthumb -T "Arduinizer_Release.ld" -o"Arduinizer.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) Arduinizer.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size Arduinizer.axf; arm-none-eabi-objcopy -O binary Arduinizer.axf Arduinizer.bin; checksum Arduinizer.bin;
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
