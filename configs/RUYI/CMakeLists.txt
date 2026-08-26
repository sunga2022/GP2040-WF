# 设置使用 ruyi 板级配置
set(PICO_BOARD ruyi)

add_executable(Pico
${CMAKE_SOURCE_DIR}/src/main.cpp)
link_libraries(${PROJECT_NAME})