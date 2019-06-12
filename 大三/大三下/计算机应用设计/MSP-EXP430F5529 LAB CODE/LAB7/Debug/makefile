################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := D:/ccs5/ccsv5/tools/compiler/msp430

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./USB/USB_User/usb_eventHandling.obj" \
"./USB/USB_User/usbConstructs.obj" \
"./USB/USB_User/UsbMscUser.obj" \
"./USB/USB_config/descriptors.obj" \
"./USB/USB_config/UsbIsr.obj" \
"./UserExperienceDemo/mouse.obj" \
"./system_pre_init.obj" \
"./lab7.obj" \
"./LAB7main.obj" \
-l"libc.a" \
"../MSP-EXP430F5529_UE.lib" \
"../lnk_msp430f5529.cmd" \
"../lnk_msp430f5529_USB.cmd" \
"../rts430xl.lib" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include UserExperienceDemo/subdir_vars.mk
-include USB/USB_config/subdir_vars.mk
-include USB/USB_User/subdir_vars.mk
-include subdir_rules.mk
-include UserExperienceDemo/subdir_rules.mk
-include USB/USB_config/subdir_rules.mk
-include USB/USB_User/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: LAB7.out

# Tool invocations
LAB7.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: MSP430 Linker'
	"D:/ccs5/ccsv5/tools/compiler/msp430/bin/cl430" -vmspx --abi=coffabi -O0 -g --gcc --define=__MSP430F5529__ --diag_warning=225 --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --large_memory_model --printf_support=minimal -z -m"LAB7.map" --stack_size=160 --heap_size=160 --use_hw_mpy=F5 -i"D:/ccs5/ccsv5/ccs_base/msp430/include" -i"D:/ccs5/ccsv5/tools/compiler/msp430/lib" -i"D:/ccs5/ccsv5/tools/compiler/msp430/include" --reread_libs --warn_sections --undef_sym=iUsbInterruptHandler --rom_model -o "LAB7.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(MSP430_EXECUTABLE_OUTPUTS__QUOTED) "LAB7.out"
	-$(RM) "LAB7main.pp" "lab7.pp" "system_pre_init.pp" "UserExperienceDemo\mouse.pp" "USB\USB_config\UsbIsr.pp" "USB\USB_config\descriptors.pp" "USB\USB_User\UsbMscUser.pp" "USB\USB_User\usbConstructs.pp" "USB\USB_User\usb_eventHandling.pp" 
	-$(RM) "LAB7main.obj" "lab7.obj" "system_pre_init.obj" "UserExperienceDemo\mouse.obj" "USB\USB_config\UsbIsr.obj" "USB\USB_config\descriptors.obj" "USB\USB_User\UsbMscUser.obj" "USB\USB_User\usbConstructs.obj" "USB\USB_User\usb_eventHandling.obj" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
