cmake_minimum_required(VERSION 3.26)
project(senkojs VERSION 0.2 LANGUAGES C CXX)

find_package(PkgConfig REQUIRED)

pkg_check_modules(SPIDERMONKE REQUIRED mozjs-102)

FILE(GLOB SRC ./src/*.cpp)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

add_executable(senkora ${SRC})
target_link_libraries(senkora ${SPIDERMONKE_LIBRARIES})
target_include_directories(senkora PUBLIC ${SPIDERMONKE_INCLUDE_DIRS})
target_compile_options(senkora PUBLIC ${SPIDERMONKE_CFLAGS_OTHER})
set(EXECUTABLE_OUTPUT_PATH ../)
