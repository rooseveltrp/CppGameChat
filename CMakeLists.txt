cmake_minimum_required(VERSION 3.16)
project(CppGameChat VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Werror)
endif()

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Network)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(core)
add_subdirectory(app)
add_subdirectory(tests)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)