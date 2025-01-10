cmake_minimum_required(VERSION 3.24...3.30)

include(FetchContent)

find_package(Git REQUIRED)

# Get emsdk files, we need the Emscripten.cmake file and this way any computer can just call config and it just works :-) ( can't make work on windows :'-C )
if ("${PLATFORM}" STREQUAL "Web" AND NOT WIN32)
    FetchContent_Declare(
        emsdk
        URL https://github.com/emscripten-core/emsdk/archive/refs/tags/3.1.74.tar.gz
        LOG_DOWNLOAD ON
    )
    FetchContent_MakeAvailable(emsdk)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/build_web/_deps/emsdk-src/upstream")
        message("Installing emsdk")
        execute_process(
            COMMAND bash -c "./_deps/emsdk-src/emsdk install latest"
            OUTPUT_VARIABLE outVar
        )
        execute_process(
            COMMAND bash -c "./_deps/emsdk-src/emsdk activate latest"
            OUTPUT_VARIABLE outVar
        )
    endif()
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/build_web/_deps/emsdk-src/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake")
endif()

# On windows emsdk will need to be at the root, replace the drive letter if needed
if ("${PLATFORM}" STREQUAL "Web" AND WIN32)
    set(CMAKE_TOOLCHAIN_FILE "C:/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake")
endif()

project(raylib-game-template)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
set(RAYLIB_VERSION 5.5)

FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    FIND_PACKAGE_ARGS
)

FetchContent_MakeAvailable(raylib)

set(RAYGUI_VERSION 4.0)

FetchContent_Declare(
    raygui
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raygui/archive/refs/tags/${RAYGUI_VERSION}.tar.gz
    FIND_PACKAGE_ARGS
)

FetchContent_MakeAvailable(raygui)

include_directories(${CMAKE_BINARY_DIR}/_deps/raygui-src/src)

# Our Project
file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/states/*.cpp
    ${CMAKE_SOURCE_DIR}/src/entities/*.cpp
)

file(GLOB_RECURSE HEADERS
    ${CMAKE_SOURCE_DIR}/src/*.h
    ${CMAKE_SOURCE_DIR}/src/states/*.h
    ${CMAKE_SOURCE_DIR}/src/entities/*.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
add_subdirectory(src)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)

if ("${PLATFORM}" STREQUAL "Web")
    add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/../resources
    )
    #DEPENDS ${PROJECT_NAME}
else()
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
    )
    #DEPENDS ${PROJECT_NAME}
endif()

#set(raylib_VERBOSE 1)
target_link_libraries(${PROJECT_NAME} raylib m)

# Web Configurations
if ("${PLATFORM}" STREQUAL "Web")
    # Tell Emscripten to build an example.html file.
    #set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
    set(CMAKE_EXECUTABLE_SUFFIX .html)
    target_link_options(${PROJECT_NAME} PUBLIC -sUSE_GLFW=3 -sMAX_WEBGL_VERSION=2 -sALLOW_MEMORY_GROWTH PUBLIC --preload-file resources -fexceptions -lbrowser.js)
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
