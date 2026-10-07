################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
build-307706554: ../empty_mspm0l2228.syscfg
	@echo 'SysConfig - building file: "$<"'
	"/home/gustavnybro/ti/sysconfig_1.26.2/sysconfig_cli.sh" -s "/home/gustavnybro/ti/mspm0_sdk_2_11_00_07/.metadata/product.json" --script "/home/gustavnybro/Projects/P7/workspace_ccstheia/P7/empty_mspm0l2228.syscfg" -o "syscfg" --compiler gcc
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/device_linker.lds: build-307706554 ../empty_mspm0l2228.syscfg
syscfg/device.opt: build-307706554
syscfg/device.lds.genlibs: build-307706554
syscfg/ti_msp_dl_config.c: build-307706554
syscfg/ti_msp_dl_config.h: build-307706554
syscfg/Event.dot: build-307706554
syscfg: build-307706554

syscfg/%.o: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'GNU Compiler - building file: "$<"'
	"/home/gustavnybro/ti/gcc_arm_none_eabi_9_2_1/bin/arm-none-eabi-gcc-9.2.1" -c @"syscfg/device.opt"  -mcpu=cortex-m0plus -march=armv6-m -mthumb -mfloat-abi=soft -I"/home/gustavnybro/Projects/P7/workspace_ccstheia/P7" -I"/home/gustavnybro/Projects/P7/workspace_ccstheia/P7/Debug" -I"/home/gustavnybro/ti/mspm0_sdk_2_11_00_07/source/third_party/CMSIS/Core/Include" -I"/home/gustavnybro/ti/mspm0_sdk_2_11_00_07/source" -I"/home/gustavnybro/ti/gcc_arm_none_eabi_9_2_1/arm-none-eabi/include/newlib-nano" -I"/home/gustavnybro/ti/gcc_arm_none_eabi_9_2_1/arm-none-eabi/include" -O2 -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -MMD -MP -MF"syscfg/$(basename $(<F)).d_raw" -MT"$(@)" -I"/home/gustavnybro/Projects/P7/workspace_ccstheia/P7/Debug/syscfg" -std=c99 $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

startup_mspm0l222x_gcc.o: /home/gustavnybro/ti/mspm0_sdk_2_11_00_07/source/ti/devices/msp/m0p/startup_system_files/gcc/startup_mspm0l222x_gcc.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'GNU Compiler - building file: "$<"'
	"/home/gustavnybro/ti/gcc_arm_none_eabi_9_2_1/bin/arm-none-eabi-gcc-9.2.1" -c @"syscfg/device.opt"  -mcpu=cortex-m0plus -march=armv6-m -mthumb -mfloat-abi=soft -I"/home/gustavnybro/Projects/P7/workspace_ccstheia/P7" -I"/home/gustavnybro/Projects/P7/workspace_ccstheia/P7/Debug" -I"/home/gustavnybro/ti/mspm0_sdk_2_11_00_07/source/third_party/CMSIS/Core/Include" -I"/home/gustavnybro/ti/mspm0_sdk_2_11_00_07/source" -I"/home/gustavnybro/ti/gcc_arm_none_eabi_9_2_1/arm-none-eabi/include/newlib-nano" -I"/home/gustavnybro/ti/gcc_arm_none_eabi_9_2_1/arm-none-eabi/include" -O2 -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)" -I"/home/gustavnybro/Projects/P7/workspace_ccstheia/P7/Debug/syscfg" -std=c99 $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'GNU Compiler - building file: "$<"'
	"/home/gustavnybro/ti/gcc_arm_none_eabi_9_2_1/bin/arm-none-eabi-gcc-9.2.1" -c @"syscfg/device.opt"  -mcpu=cortex-m0plus -march=armv6-m -mthumb -mfloat-abi=soft -I"/home/gustavnybro/Projects/P7/workspace_ccstheia/P7" -I"/home/gustavnybro/Projects/P7/workspace_ccstheia/P7/Debug" -I"/home/gustavnybro/ti/mspm0_sdk_2_11_00_07/source/third_party/CMSIS/Core/Include" -I"/home/gustavnybro/ti/mspm0_sdk_2_11_00_07/source" -I"/home/gustavnybro/ti/gcc_arm_none_eabi_9_2_1/arm-none-eabi/include/newlib-nano" -I"/home/gustavnybro/ti/gcc_arm_none_eabi_9_2_1/arm-none-eabi/include" -O2 -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)" -I"/home/gustavnybro/Projects/P7/workspace_ccstheia/P7/Debug/syscfg" -std=c99 $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


