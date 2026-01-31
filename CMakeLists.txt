cmake_minimum_required(VERSION 3.14.0)

project(vbr VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})

set(TARGET ${PROJECT_NAME})

find_package(SDL3 REQUIRED)
find_package(Vulkan REQUIRED)


set(SOURCES src/main.cpp)

add_executable(${TARGET} ${SOURCES})

target_link_libraries(${TARGET}
  ${SDL3_LIBRARIES}
  ${Vulkan_LIBRARIES})
