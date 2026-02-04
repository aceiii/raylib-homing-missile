cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME tigr-test)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCE_FILES
    src/main.cpp
    src/vec2.cpp
    src/missiles.cpp
    src/random.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

add_subdirectory(external/raylib)
add_subdirectory(external/spdlog)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib spdlog::spdlog)
