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
find_package(glm REQUIRED)
find_package(spdlog REQUIRED)

# compile lib
set(LIB_SOURCES
  src/base/image.cpp
  src/base/util.cpp
  src/base/device.cpp
  src/base/swapchain.cpp
  src/base/base.cpp
  src/base/buffer.cpp
  src/base/descriptor.cpp
  src/base/layout.cpp
  src/base/graphics_pipeline.cpp
)

add_library(${PROJECT_NAME} STATIC ${LIB_SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC
  ${SDL3_LIBRARIES}
  ${Vulkan_LIBRARIES}
  glm::glm
  spdlog::spdlog
)

# generate exe
# base triangle
set(BASE_TRIANGLE_SOURCE
  tests/base_triangle/base_triangle.cpp
  tests/base_triangle/main.cpp)

add_executable(base_triangle ${BASE_TRIANGLE_SOURCE})
target_link_libraries(base_triangle vbr)

# buffer triangle
set(BUFFER_TRIANGLE_SOURCE
  tests/buffer_triangle/buffer_triangle.cpp
  tests/buffer_triangle/main.cpp)

add_executable(buffer_triangle ${BUFFER_TRIANGLE_SOURCE})
target_link_libraries(buffer_triangle vbr)

# index buffer
set(INDEX_BUFFER_SOURCE
  tests/index_buffer/index_buffer.cpp
  tests/index_buffer/main.cpp)

add_executable(index ${INDEX_BUFFER_SOURCE})
target_link_libraries(index vbr)

# uniform buffer
set(UNIFORM_SOURCE
  tests/uniform/uniform.cpp
  tests/uniform/main.cpp)

add_executable(uniform ${UNIFORM_SOURCE})
target_link_libraries(uniform vbr)

# texture
set(TEXTURE_SOURCE
  tests/texture/texture.cpp
  tests/texture/main.cpp)

add_executable(texture ${TEXTURE_SOURCE})
target_link_libraries(texture vbr)
