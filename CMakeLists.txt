cmake_minimum_required(VERSION 3.22)
project(senkora VERSION 0.2 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic")

find_package(PkgConfig REQUIRED)

pkg_check_modules(V8 IMPORTED_TARGET REQUIRED v8)

FILE(GLOB_RECURSE SRC ./src/*.cpp ./src/*.c ./src/*.h ./src/*.hpp)

execute_process(COMMAND make 
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libs/libFoxEvents/)

link_directories(${V8_LIBRARY_DIRS})
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/libFoxEvents/)
include_directories(./src/api/)
include_directories(./libs/libFoxEvents/src/)

add_executable(senkora ${SRC})
target_link_libraries(senkora foxevents)
add_compile_definitions(V8_COMPRESS_POINTERS)
target_link_libraries(senkora ${V8_LIBRARIES})
target_include_directories(senkora PUBLIC ${V8_INCLUDE_DIRS})
target_compile_options(senkora PUBLIC ${V8_CFLAGS_OTHER})
set(EXECUTABLE_OUTPUT_PATH ../dist)
