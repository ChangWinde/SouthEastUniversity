################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
USB/USB_config/%.obj: ../USB/USB_config/%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"D:/tempSoftWare/ccs/tools/compiler/msp430_4.0.2/bin/cl430" -vmspx --abi=coffabi -O0 -g --include_path="D:/tempSoftWare/ccs/ccs_base/msp430/include" --include_path="D:/tempSoftWare/ccs/tools/compiler/msp430_4.0.2/include" --include_path="D:/WorkPlace/LAB1/F5xx_F6xx_Core_Lib" --include_path="D:/WorkPlace/LAB1/MSP-EXP430F5529_HAL" --include_path="D:/WorkPlace/LAB1/FatFs" --include_path="D:/WorkPlace/LAB1/CTS" --include_path="D:/WorkPlace/LAB1/USB" --include_path="D:/WorkPlace/LAB1/USB/USB_API/USB_CDC_API" --include_path="D:/WorkPlace/LAB1/USB/USB_API/USB_Common" --include_path="D:/WorkPlace/LAB1/USB/USB_API/USB_HID_API" --include_path="D:/WorkPlace/LAB1/USB/USB_User" --include_path="D:/WorkPlace/LAB1/USB/USB_API" --include_path="D:/WorkPlace/LAB1/USB/USB_config" --include_path="D:/WorkPlace/LAB1/UserExperienceDemo" --include_path="D:/WorkPlace/LAB1/UserExperienceDemo/Puzzle" --gcc --define=__MSP430F5529__ --diag_warning=225 --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --large_memory_model --printf_support=minimal --preproc_with_compile --preproc_dependency="USB/USB_config/$(basename $(<F)).d_raw" --obj_directory="USB/USB_config" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


