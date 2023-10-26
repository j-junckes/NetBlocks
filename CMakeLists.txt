cmake_minimum_required(VERSION 3.26)
project(NetBlocks)

set(CMAKE_CXX_STANDARD 23)
SET(BUILD_ENV "" CACHE STRING "Current build environment (DESKTOP, WEB)")

string(TOUPPER ${BUILD_ENV} BUILD_ENV)
add_definitions(-DPLATFORM_${BUILD_ENV})

list(APPEND CORE_BUILD_FILES
  src/main.cpp
)

add_executable(NetBlocks ${CORE_BUILD_FILES}
  src/Chunk.cpp
  src/Chunk.hpp
  src/Mesh.hpp
  src/gl.hpp
  src/Shader.cpp
  src/Shader.hpp
  src/Exit.hpp
  src/Exit.cpp
  src/Input.cpp
  src/Input.hpp
  src/Camera.cpp
  src/Camera.hpp)

if (BUILD_ENV STREQUAL "WEB")
  set_target_properties(NetBlocks
    PROPERTIES SUFFIX ".html"
    LINK_FLAGS "-O2 -sUSE_SDL=2 -sALLOW_MEMORY_GROWTH=1 -sUSE_WEBGL2=1 -sFULL_ES3=1 -sWASM=1 \
     -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/src/web/NetBlocks.html \
     --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets"
  )

  target_include_directories(NetBlocks PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/external
  )

  em_link_js_library(NetBlocks ${CMAKE_CURRENT_SOURCE_DIR}/src/web/NetBlocksLib.js)
elseif (BUILD_ENV STREQUAL "DESKTOP")
  find_package(SDL2 CONFIG REQUIRED)
  find_package(GLEW REQUIRED)
  find_package(OpenGL REQUIRED)
  find_package(glm CONFIG REQUIRED)

  target_link_libraries(NetBlocks PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    GLEW::GLEW
    OpenGL::GL
    glm::glm
  )

  add_custom_target(copy-runtime-files ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets
    DEPENDS NetBlocks
  )
endif ()
