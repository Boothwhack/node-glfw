cmake_minimum_required(VERSION 3.20)
project(node_glfw)

set(CMAKE_CXX_STANDARD 17)

add_library(node_glfw SHARED ${CMAKE_JS_SRC} src/window.cpp src/window.hpp src/addon.cpp)
set_target_properties(node_glfw PROPERTIES PREFIX "" SUFFIX ".node")

execute_process(COMMAND node -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_INCLUDE_DIR)
string(REPLACE "\n" "" NODE_ADDON_API_INCLUDE_DIR ${NODE_ADDON_API_INCLUDE_DIR})
string(REPLACE "\"" "" NODE_ADDON_API_INCLUDE_DIR ${NODE_ADDON_API_INCLUDE_DIR})

target_include_directories(node_glfw PRIVATE ${CMAKE_JS_INC} ${NODE_ADDON_API_INCLUDE_DIR} src)
target_link_libraries(node_glfw PUBLIC ${CMAKE_JS_LIB})

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

target_link_libraries(node_glfw PUBLIC glfw)
