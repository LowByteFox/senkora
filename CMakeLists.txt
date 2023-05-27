cmake_minimum_required(VERSION 3.22)
project(senkora VERSION 0.2 LANGUAGES C CXX)


set(CMAKE_EXPORT_COMPILE_COMMANDS true)

find_package(PkgConfig REQUIRED)

pkg_check_modules(SPIDERMONKE IMPORTED_TARGET REQUIRED mozjs-115a1)

FILE(GLOB SRC ./src/*.cpp ./src/api/*.cpp)

add_executable(senkora ${SRC})
target_link_libraries(senkora ${SPIDERMONKE_LIBRARIES})
target_include_directories(senkora PUBLIC ${SPIDERMONKE_INCLUDE_DIRS})
target_include_directories(senkora PUBLIC ./src/api/)
target_compile_options(senkora PUBLIC ${SPIDERMONKE_CFLAGS_OTHER})
set(EXECUTABLE_OUTPUT_PATH ../dist)
