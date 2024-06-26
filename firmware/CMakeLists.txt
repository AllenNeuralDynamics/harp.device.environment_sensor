cmake_minimum_required(VERSION 3.13)

include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

project(env_sensor)

set(CMAKE_CXX_STANDARD 17)

# Enable try/catch exception interface.
#set(PICO_CXX_ENABLE_EXCEPTIONS 1)


# Compile for profiling/debugging/etc. Default: none enabled.
#add_definitions(-DDEBUG)
#add_definitions(-DDEBUG_HARP_MSG_IN)
#add_definitions(-DDEBUG_HARP_MSG_OUT)
#add_definitions(-DPROFILE_CPU)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# Add Harp Core as separate library.
add_subdirectory(lib/harp.core.rp2040/firmware)

add_executable(${PROJECT_NAME}
    src/main.cpp
)

add_library(bme688
    src/bme688.cpp
)

# Specify where to look for header files if they're not all in the same place.
#target_include_directories(${PROJECT_NAME} PUBLIC inc)
# Specify where to look for header files if they're all in one place.
include_directories(inc)

# Link libraries to the targets that need them.
target_link_libraries(bme688 hardware_spi pico_stdlib)
target_link_libraries(${PROJECT_NAME} bm688 pico_stdlib
                      core1_lick_detection pico_multicore harp_sync harp_c_app)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(${PROJECT_NAME})

# USB serial. Enable this for every library/executable that uses it.
#pico_enable_stdio_usb(${PROJECT_NAME} 1)

if(DEBUG)
    message(WARNING "Debug printf() messages enabled from harp core to UART \
            with baud rate 921600.")
    pico_enable_stdio_uart(${PROJECT_NAME} 1)
    pico_enable_stdio_uart(bme688 1)
endif()
