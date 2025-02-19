cmake_minimum_required(VERSION 3.13)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 1.5.1)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(bitDogMat C CXX ASM)




# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(bitDogMat bitDogMat.c 
                ssd1306_i2c.c
)
        


pico_set_program_name(bitDogMat "bitDogMat")
pico_set_program_version(bitDogMat "0.1")

# Generate PIO header
pico_generate_pio_header(bitDogMat ${CMAKE_CURRENT_LIST_DIR}/ws2818b.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(bitDogMat 0)
pico_enable_stdio_usb(bitDogMat 1)

# Add the standard library to the build
target_link_libraries(bitDogMat
        pico_stdlib
        hardware_adc
        hardware_pwm
        
        )

# Add the standard include files to the build
target_include_directories(bitDogMat PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(bitDogMat 
        hardware_pio
        hardware_clocks
        hardware_i2c
        )

pico_add_extra_outputs(bitDogMat)
