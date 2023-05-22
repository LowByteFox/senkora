cmake_minimum_required(VERSION 3.22)
project(senkora VERSION 0.2 LANGUAGES C CXX)

find_package(PkgConfig REQUIRED)

pkg_check_modules(SPIDERMONKE IMPORTED_TARGET REQUIRED mozjs-102)

FILE(GLOB SRC ./src/*.cpp)
FILE(GLOB API ./src/api/*.cpp)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

add_library(senkora ${API})
target_link_libraries(senkora ${SPIDERMONKE_LIBRARIES})
target_include_directories(senkora PUBLIC ${SPIDERMONKE_INCLUDE_DIRS})
target_compile_options(senkora PUBLIC ${SPIDERMONKE_CFLAGS_OTHER})

add_executable(main ${SRC})
target_link_libraries(main ${SPIDERMONKE_LIBRARIES})
target_link_libraries(main senkora)
target_include_directories(main PUBLIC ${SPIDERMONKE_INCLUDE_DIRS})
target_include_directories(main PUBLIC ./src/api/)
target_compile_options(main PUBLIC ${SPIDERMONKE_CFLAGS_OTHER})
set(EXECUTABLE_OUTPUT_PATH ../dist)
