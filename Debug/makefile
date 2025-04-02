################################################################################
# 自动生成的文件。不要编辑！
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include user/subdir.mk
-include libraries/zf_driver/subdir.mk
-include libraries/zf_device/subdir.mk
-include libraries/zf_components/subdir.mk
-include libraries/zf_common/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/_PinMap/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/_Lib/DataHandling/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/_Impl/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Vadc/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Vadc/Adc/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Stm/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Src/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Scu/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Qspi/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Qspi/SpiMaster/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Port/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Mtu/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Gtm/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Gtm/Atom/Pwm/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Gpt12/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Gpt12/IncrEnc/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Flash/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Dma/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Dma/Dma/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Cpu/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Cpu/Irq/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Cpu/CStart/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Ccu6/Timer/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Ccu6/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Asclin/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Asclin/Asc/subdir.mk
-include libraries/infineon_libraries/Service/CpuGeneric/SysSe/Math/subdir.mk
-include libraries/infineon_libraries/Service/CpuGeneric/SysSe/Bsp/subdir.mk
-include libraries/infineon_libraries/Service/CpuGeneric/StdIf/subdir.mk
-include libraries/infineon_libraries/Service/CpuGeneric/If/subdir.mk
-include libraries/infineon_libraries/Infra/Platform/Tricore/Compilers/subdir.mk
-include code/subdir.mk
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := E07_09_wifi_spi_mt9v03x_demo
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_SIZE += \
E07_09_wifi_spi_mt9v03x_demo.siz \


# 所有目标
all: main-build

# 主构建目标
main-build: E07_09_wifi_spi_mt9v03x_demo.elf secondary-outputs

# 工具调用
E07_09_wifi_spi_mt9v03x_demo.elf: $(OBJS) makefile $(OPTIONAL_TOOL_DEPS)
	@echo '正在构建目标： $@'
	@echo '正在调用： TASKING Linker'
	cctc --lsl-file="../Lcf_Tasking_Tricore_Tc.lsl" -Wl-Oc -Wl-OL -Wl-Ot -Wl-Ox -Wl-Oy "D:\ADS_DESKWorks\Leading_Car\Tandem_Racing_Azure_Steed_in_Pursuit\libraries\zf_device\zf_device_config.a" -Wl--map-file="E07_09_wifi_spi_mt9v03x_demo.map" -Wl-mc -Wl-mf -Wl-mi -Wl-mk -Wl-ml -Wl-mm -Wl-md -Wl-mr -Wl-mu --no-warnings= -Wl--error-limit=42 --fp-model=3 -lcs_fpu -lfp_fpu -lrt --lsl-core=vtc --exceptions --strict --anachronisms --force-c++ -Ctc26xb -o"E07_09_wifi_spi_mt9v03x_demo.elf" -Wl-o"E07_09_wifi_spi_mt9v03x_demo.hex:IHEX" $(OBJS)
	@echo '已结束构建目标： $@'
	@echo ' '

E07_09_wifi_spi_mt9v03x_demo.siz: E07_09_wifi_spi_mt9v03x_demo.elf makefile $(OPTIONAL_TOOL_DEPS)
	@echo '正在调用： Print Size'
	elfsize  "E07_09_wifi_spi_mt9v03x_demo.elf"
	@echo '已结束构建： $@'
	@echo ' '

# 其他目标
clean:
	find . ! -name "*.mk" ! -name "makefile" ! -name "." -type f -delete 
	-@echo ' '

secondary-outputs: $(SECONDARY_SIZE)

.PHONY: all clean dependents main-build

-include ../makefile.targets
