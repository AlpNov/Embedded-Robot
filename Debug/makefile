################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv5/tools/compiler/arm_5.1.1

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./usr_include/usr_uart.obj" \
"./usr_include/ultrasonic.obj" \
"./usr_include/shaft_encoder.obj" \
"./usr_include/motor.obj" \
"./usr_include/modbus.obj" \
"./usr_include/line.obj" \
"./usr_include/functions.obj" \
"./usr_include/color.obj" \
"./usr_include/cmd_interp.obj" \
"./usr_include/IRdist.obj" \
"./utils/uartstdio.obj" \
"./main.obj" \
"./lm4f120h5qr_startup_ccs.obj" \
"../lm4f120h5qr.cmd" \
-l"libc.a" \
-l"C:\StellarisWare\driverlib\ccs-cm4f\Debug\driverlib-cm4f.lib" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include utils/subdir_vars.mk
-include usr_include/subdir_vars.mk
-include subdir_rules.mk
-include utils/subdir_rules.mk
-include usr_include/subdir_rules.mk
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
all: Team_14-Nguyen_Povolotskaya.out

# Tool invocations
Team_14-Nguyen_Povolotskaya.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Linker'
	"C:/ti/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -g --define=PART_LM4F120H5QR --diag_warning=225 --display_error_number --diag_wrap=off -z -m"Team_14-Nguyen_Povolotskaya.map" -i"C:/ti/ccsv5/tools/compiler/arm_5.1.1/lib" -i"C:/StellarisWare/" -i"C:/ti/ccsv5/tools/compiler/arm_5.1.1/include" --reread_libs --warn_sections --display_error_number --diag_wrap=off --xml_link_info="Team_14-Nguyen_Povolotskaya_linkInfo.xml" --rom_model -o "Team_14-Nguyen_Povolotskaya.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(TMS470_EXECUTABLE_OUTPUTS__QUOTED) "Team_14-Nguyen_Povolotskaya.out"
	-$(RM) "lm4f120h5qr_startup_ccs.pp" "main.pp" "utils\uartstdio.pp" "usr_include\IRdist.pp" "usr_include\cmd_interp.pp" "usr_include\color.pp" "usr_include\functions.pp" "usr_include\line.pp" "usr_include\modbus.pp" "usr_include\motor.pp" "usr_include\shaft_encoder.pp" "usr_include\ultrasonic.pp" "usr_include\usr_uart.pp" 
	-$(RM) "lm4f120h5qr_startup_ccs.obj" "main.obj" "utils\uartstdio.obj" "usr_include\IRdist.obj" "usr_include\cmd_interp.obj" "usr_include\color.obj" "usr_include\functions.obj" "usr_include\line.obj" "usr_include\modbus.obj" "usr_include\motor.obj" "usr_include\shaft_encoder.obj" "usr_include\ultrasonic.obj" "usr_include\usr_uart.obj" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

