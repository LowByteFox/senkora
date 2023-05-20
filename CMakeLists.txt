cmake_minimum_required(VERSION 3.26)
project(senkojs VERSION 0.2 LANGUAGES CXX)

find_package(PkgConfig REQUIRED)

pkg_check_modules(SPIDERMONKE REQUIRED mozjs-102)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)

add_executable(hello ./hello.cpp ./boilerplate.cpp)
target_link_libraries(hello ${SPIDERMONKE_LIBRARIES})
target_include_directories(hello PUBLIC ${SPIDERMONKE_INCLUDE_DIRS})
target_compile_options(hello PUBLIC ${SPIDERMONKE_CFLAGS_OTHER})
