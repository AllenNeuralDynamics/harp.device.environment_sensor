cmake_minimum_required(VERSION 3.13)
find_package(Git REQUIRED)
execute_process(COMMAND "${GIT_EXECUTABLE}" rev-parse --short HEAD OUTPUT_VARIABLE COMMIT_ID OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "Computed Git Hash: ${COMMIT_ID}")
add_definitions(-DGIT_HASH="${COMMIT_ID}") # Usable in source code.

# Specify USB Manufacturer and Product descriptions.
add_definitions(-DUSBD_MANUFACTURER="Allen Institute")
add_definitions(-DUSBD_PRODUCT="Environment Sensor")

include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

project(env_sensor)

set(CMAKE_CXX_STANDARD 17)

# Enable try/catch exception interface.
#set(PICO_CXX_ENABLE_EXCEPTIONS 1)


# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# # Add Harp Core as separate library.
add_subdirectory(lib/harp.core.rp2040/firmware)

if(NOT DEBUG)
    add_executable(${PROJECT_NAME}
        src/main.cpp
    )
else()
    add_executable(${PROJECT_NAME}
    src/main_debug_noharp.cpp
    )
endif()

add_library(bme68x
    src/bme68x.c
)
add_library(Adafruit_BME680_rpi
    src/Adafruit_BME680_rpi.cpp
)
add_library(myspi
    src/myspi.cpp
)
add_library(core1_sensor
    src/core1_sensor.cpp
)

##############

# Specify where to look for header files if they're not all in the same place.
#target_include_directories(${PROJECT_NAME} PUBLIC inc)
# Specify where to look for header files if they're all in one place.
include_directories(inc)

# Link libraries to the targets that need them.
target_link_libraries(myspi hardware_spi pico_stdlib)
target_link_libraries(Adafruit_BME680_rpi pico_stdlib bme68x myspi)

if(NOT DEBUG)
    target_link_libraries(core1_sensor Adafruit_BME680_rpi)
    target_link_libraries(${PROJECT_NAME} pico_stdlib pico_multicore harp_sync harp_c_app core1_sensor)
else() 
    target_link_libraries(${PROJECT_NAME} Adafruit_BME680)
endif()


# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(${PROJECT_NAME})

if(DEBUG)
    message(WARNING "Debug printf() messages enabled from harp core to UART \
            with baud rate 921600.")
    pico_enable_stdio_usb(${PROJECT_NAME} 1)
    pico_enable_stdio_uart(${PROJECT_NAME} 0)
            
endif()
