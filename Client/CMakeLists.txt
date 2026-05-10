cmake_minimum_required(VERSION 3.10)
project(Tetris)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TARGET_NAME "Tetris")

set(SOURCES
  src/main.cpp
  src/render/render.cpp
  src/render/ui.cpp
  src/window/window.cpp
  src/window/tetris_window/tetris_window.cpp
  src/window/super_tetris_window/super_tetris_window.cpp
  src/window/super_puper_tetris_window/super_puper_tetris_window.cpp
  src/tetris/tetris.cpp
  src/tetris/tetris_field.cpp
  src/tetris/tetris_shapes.cpp
  src/tetris/bot/bots_client.cpp
  src/tetris/bot/200iq_bot.cpp
  src/tetris/manual/manual_tetris.cpp
  src/tetris/stats_sender/stats_sender.cpp
  src/utils/audio.cpp
)

set(MINIFB_BUILD_EXAMPLES OFF CACHE BOOL "")

add_subdirectory(third_party/minifb)

set(USE_TLS OFF CACHE BOOL "" FORCE)
set(USE_WS OFF CACHE BOOL "" FORCE)
set(IXWEBSOCKET_INSTALL OFF CACHE BOOL "" FORCE)
set(USE_ZLIB OFF CACHE BOOL "" FORCE)

add_subdirectory(third_party/ixwebsocket)
if (MSVC)
  target_compile_options(ixwebsocket PRIVATE /w)
endif()

add_executable(${TARGET_NAME} ${SOURCES})

target_link_libraries(${TARGET_NAME}
  PRIVATE
  minifb
  ixwebsocket
)

target_include_directories(${TARGET_NAME}
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/minifb
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/ixwebsocket
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/httplib
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/json/include
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/miniaudio
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Compile options
if(MSVC)
  target_compile_options(${TARGET_NAME} PRIVATE /W4 /std:c++20)
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
    -mrdrnd
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

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/bin" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

message(STATUS "Minifb from: ${CMAKE_CURRENT_SOURCE_DIR}/minifb")
