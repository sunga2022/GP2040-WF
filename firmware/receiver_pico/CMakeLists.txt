cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD pico CACHE STRING "Board type")

include(${CMAKE_CURRENT_LIST_DIR}/../../pico_sdk_import.cmake)
project(gp2040wf_receiver C CXX ASM)
pico_sdk_init()

add_executable(gp2040wf_receiver
  main.c
  usb_descriptors.c
  ${CMAKE_CURRENT_LIST_DIR}/../../wireless/nrf24.c
)

target_include_directories(gp2040wf_receiver PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/../../wireless
)

target_link_libraries(gp2040wf_receiver
  pico_stdlib
  pico_unique_id
  tinyusb_device
  tinyusb_board
  hardware_gpio
)

pico_enable_stdio_usb(gp2040wf_receiver 0)
pico_enable_stdio_uart(gp2040wf_receiver 0)
pico_add_extra_outputs(gp2040wf_receiver)
