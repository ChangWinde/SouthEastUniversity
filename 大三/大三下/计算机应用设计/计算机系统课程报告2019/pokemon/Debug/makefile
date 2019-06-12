################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := D:/tempSoftWare/ccs/tools/compiler/msp430_4.0.2

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./LAB1main.obj" \
"./LCD_128.obj" \
"./lab1.obj" \
"./system_pre_init.obj" \
"./USB/USB_User/UsbMscUser.obj" \
"./USB/USB_User/usbConstructs.obj" \
"./USB/USB_User/usb_eventHandling.obj" \
"./USB/USB_config/UsbIsr.obj" \
"./USB/USB_config/descriptors.obj" \
"../MSP-EXP430F5529_UE.lib" \
"../lnk_msp430f5529.cmd" \
"../lnk_msp430f5529_USB.cmd" \
"../rts430xl.lib" \
$(GEN_CMDS__FLAG) \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include USB/USB_User/subdir_vars.mk
-include USB/USB_config/subdir_vars.mk
-include subdir_rules.mk
-include USB/USB_User/subdir_rules.mk
-include USB/USB_config/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
LAB1.out \

EXE_OUTPUTS__QUOTED += \
"LAB1.out" \


# All Target
all: LAB1.out

# Tool invocations
LAB1.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: MSP430 Linker'
	"D:/tempSoftWare/ccs/tools/compiler/msp430_4.0.2/bin/cl430" -vmspx --abi=coffabi -O0 -g --gcc --define=__MSP430F5529__ --diag_warning=225 --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --large_memory_model --printf_support=minimal -z -m"LAB1.map" --stack_size=160 --heap_size=160 --use_hw_mpy=F5 -i"D:/tempSoftWare/ccs/ccs_base/msp430/include" -i"D:/tempSoftWare/ccs/tools/compiler/msp430_4.0.2/lib" -i"D:/tempSoftWare/ccs/tools/compiler/msp430_4.0.2/include" --reread_libs --warn_sections --xml_link_info="LAB1_linkInfo.xml" --undef_sym=iUsbInterruptHandler --rom_model -o "LAB1.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)
	-$(RM) "LAB1main.obj" "LCD_128.obj" "lab1.obj" "system_pre_init.obj" "USB\USB_User\UsbMscUser.obj" "USB\USB_User\usbConstructs.obj" "USB\USB_User\usb_eventHandling.obj" "USB\USB_config\UsbIsr.obj" "USB\USB_config\descriptors.obj" 
	-$(RM) "LAB1main.d" "LCD_128.d" "lab1.d" "system_pre_init.d" "USB\USB_User\UsbMscUser.d" "USB\USB_User\usbConstructs.d" "USB\USB_User\usb_eventHandling.d" "USB\USB_config\UsbIsr.d" "USB\USB_config\descriptors.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

