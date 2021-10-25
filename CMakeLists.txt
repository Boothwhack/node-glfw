cmake_minimum_required(VERSION 3.20)
project(node_glfw)

set(CMAKE_CXX_STANDARD 17)

# Load GLFW sources
include(FetchContent)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG 814b7929c5add4b0541ccad26fb81f28b71dc4d8 # v3.3.4
)
FetchContent_MakeAvailable(glfw)

# Generate sources
add_custom_command(OUTPUT src/constants.cpp index.d.ts
        COMMAND
        node ${CMAKE_CURRENT_LIST_DIR}/generate-constants.js
        ${glfw_SOURCE_DIR}/include/GLFW/glfw3.h

        ${CMAKE_CURRENT_LIST_DIR}/src/constants.cpp.in
        src/constants.cpp

        ${CMAKE_CURRENT_LIST_DIR}/index.in.d.ts
        index.d.ts)

add_library(node_glfw SHARED
        index.d.ts

        src/addon.cpp
        src/constants.cpp
        src/constants.hpp
        src/window.cpp
        src/window.hpp
        src/utils.hpp)

find_package(NodeApi 16 REQUIRED)
find_package(NodeAddonApi REQUIRED)

target_include_directories(node_glfw PRIVATE src)
target_link_libraries(node_glfw PUBLIC glfw NodeApi NodeAddonApi)

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/dist")
install(TARGETS node_glfw
        CONFIGURATIONS Debug
        LIBRARY
        DESTINATION "Debug/${NODE_PLATFORM}/${NODE_ARCH}"
        RUNTIME
        DESTINATION "Debug/${NODE_PLATFORM}/${NODE_ARCH}")
install(TARGETS node_glfw
        CONFIGURATIONS Release
        LIBRARY
        DESTINATION "Release/${NODE_PLATFORM}/${NODE_ARCH}"
        RUNTIME
        DESTINATION "Release/${NODE_PLATFORM}/${NODE_ARCH}")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/index.d.ts" DESTINATION .)
