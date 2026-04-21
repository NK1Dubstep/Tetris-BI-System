cmake_minimum_required(VERSION 3.10)
project(Tetris)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TARGET_NAME "Tetris")

set(SOURCES
  src/main.cpp
  src/render/render.cpp
  src/window/window.cpp
  src/window/tetris_window/tetris_window.cpp
  src/window/super_tetris_window/super_tetris_window.cpp
  src/tetris/tetris.cpp
  src/tetris/tetris_field.cpp
  src/tetris/tetris_shapes.cpp
  src/tetris/bot/bot.cpp
)

set(MINIFB_BUILD_EXAMPLES OFF CACHE BOOL "")

add_subdirectory(minifb)

add_executable(${TARGET_NAME} ${SOURCES})

target_link_libraries(${TARGET_NAME}
  PRIVATE
  minifb
)

target_include_directories(${TARGET_NAME}
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/minifb
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Compile options
if(MSVC)
  target_compile_options(${TARGET_NAME} PRIVATE /W4)
  #[[
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${TARGET_NAME} PRIVATE
      /fsanitize=address
    )
  endif()
  #]]
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(${TARGET_NAME} PRIVATE
    -Wall
    -Wextra
    -Wpedantic
  )
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${TARGET_NAME} PRIVATE
      -fsanitize=address
      -fsanitize=undefined
      -fsanitize=leak
      -fno-omit-frame-pointer
      -g
    )
    target_link_options(${TARGET_NAME} PRIVATE
      -fsanitize=address
      -fsanitize=undefined
      -fsanitize=leak
    )
  endif()
endif()

option(NK1AV1_ENABLE_THREAD_SANITIZER "Enable ThreadSanitizer" OFF)

if(NK1AV1_ENABLE_THREAD_SANITIZER AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(${TARGET_NAME} PRIVATE -fsanitize=thread)
  target_link_options(${TARGET_NAME} PRIVATE -fsanitize=thread)
endif()

set_target_properties(${TARGET_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

message(STATUS "Minifb from: ${CMAKE_CURRENT_SOURCE_DIR}/minifb")
