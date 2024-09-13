cmake_minimum_required(VERSION 3.13)

include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

project(env_sensor)

set(CMAKE_CXX_STANDARD 17)

# Enable try/catch exception interface.
#set(PICO_CXX_ENABLE_EXCEPTIONS 1)


# Compile for profiling/debugging/etc. Default: none enabled.

set(DEBUG True)    ############### Controls whether usb is harp or printf

# add_definitions(-DDEBUG)
# set(DEBUG True)
#add_definitions(-DDEBUG_HARP_MSG_IN)
#add_definitions(-DDEBUG_HARP_MSG_OUT)
#add_definitions(-DPROFILE_CPU)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

if(NOT DEBUG)
    # Add Harp Core as separate library.
    add_subdirectory(lib/harp.core.rp2040/firmware)
endif()

# include_directories(lib/Adafruit_BME680)
# include_directories(lib/Adafruit_BusIO)

add_executable(${PROJECT_NAME}
    src/main.cpp
)

add_library(bme68x
    src/bme68x.c
)
add_library(Adafruit_BME680_rpi
    src/Adafruit_BME680_rpi.cpp
)
add_library(myspi
    src/myspi.cpp
)

# Specify where to look for header files if they're not all in the same place.
#target_include_directories(${PROJECT_NAME} PUBLIC inc)
# Specify where to look for header files if they're all in one place.
include_directories(inc)

# Link libraries to the targets that need them.
target_link_libraries(myspi hardware_spi pico_stdlib)
target_link_libraries(Adafruit_BME680_rpi hardware_spi pico_stdlib bme68x myspi)
target_link_libraries(${PROJECT_NAME} Adafruit_BME680_rpi pico_stdlib
                    #   pico_multicore harp_sync harp_c_app
)
# target_link_libraries(${PROJECT_NAME} bme688 pico_stdlib)

if(NOT DEBUG)
    target_link_libraries(${PROJECT_NAME} pico_multicore harp_sync harp_c_app)
endif()


# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(${PROJECT_NAME})

# USB serial. Enable this for every library/executable that uses it.
# pico_enable_stdio_usb(${PROJECT_NAME} 1)
# pico_enable_stdio_usb(${PROJECT_NAME} 1)
# pico_enable_stdio_uart(${PROJECT_NAME} 0)

if(DEBUG)
    message(WARNING "Debug printf() messages enabled from harp core to UART \
            with baud rate 921600.")
    pico_enable_stdio_usb(${PROJECT_NAME} 1)
    pico_enable_stdio_uart(${PROJECT_NAME} 0)
            
    # enable usb output, disable uart output
    # pico_enable_stdio_usb(${PROJECT_NAME} 1)
    # pico_enable_stdio_uart(${PROJECT_NAME} 1)
    # pico_enable_stdio_uart(bme688 1)
    # example_auto_set_url(${PROJECT_NAME})
endif()


#######################################

# cmake_minimum_required(VERSION 3.12)
# include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

# project(env_sensor)

# set(CMAKE_CXX_STANDARD 17)

# # Initialize the SDK
# pico_sdk_init()

# add_executable(env_sensor
# src/main.cpp
# )

# # pull in common dependencies
# target_link_libraries(env_sensor pico_stdlib)

# # enable usb output, disable uart output
# pico_enable_stdio_usb(env_sensor 1)
# pico_enable_stdio_uart(env_sensor 0)

# # create map/bin/hex/uf2 file etc.
# pico_add_extra_outputs(env_sensor)