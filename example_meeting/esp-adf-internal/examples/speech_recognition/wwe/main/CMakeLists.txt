set(COMPONENT_SRCDIRS  . ui image font)

set(COMPONENT_ADD_INCLUDEDIRS . include)

set(COMPONENT_REQUIRES  lvgl__lvgl audio_sal audio_stream audio_hal audio_board audio_recorder esp_peripherals audio_flash_tone adf_utils)


register_component()


# Some of warinings from LVGL. Block them.
target_compile_options(${COMPONENT_LIB} PRIVATE -w)

# We're using Kconfig, will will add `CONFIG_` prifix to MACRO.
# This macro will use a wrapper macro like `#define LV_USE_XXX CONFIG_LV_USE_XXX`
# But since `lv_conf_internal.h` will add `_LV_KCONFIG_PRESENT` if
#   `CONFIG_LV_COLOR_DEPTH` is defined. So this might not required.
# target_compile_definitions(${COMPONENT_LIB} PUBLIC "-D_LV_KCONFIG_PRESENT")

# Use simple include
target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLV_CONF_INCLUDE_SIMPLE")
target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLV_DEMO_CONF_INCLUDE_SIMPLE")

# Workaround for file system support
target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DCONFIG_LV_USE_FS_STDIO='S'")

if (CONFIG_LV_MEM_CUSTOM)
    if (CONFIG_LV_MEM_CUSTOM_ALLOC)
        target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLV_MEM_CUSTOM_ALLOC=${CONFIG_LV_MEM_CUSTOM_ALLOC}")
    endif()

    if (CONFIG_LV_MEM_CUSTOM_FREE)
        target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLV_MEM_CUSTOM_FREE=${CONFIG_LV_MEM_CUSTOM_FREE}")
    endif()
endif()

if (CONFIG_LV_TICK_CUSTOM)
    if (CONFIG_LV_TICK_CUSTOM_SYS_TIME_EXPR)
        target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLV_TICK_CUSTOM_SYS_TIME_EXPR=${CONFIG_LV_TICK_CUSTOM_SYS_TIME_EXPR}")
    endif()
endif()

# This will make LVGL faster but use more RAM if enabled
if (CONFIG_LV_ATTRIBUTE_FAST_MEM_USE_IRAM)
    target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLV_ATTRIBUTE_FAST_MEM=IRAM_ATTR")
endif()
