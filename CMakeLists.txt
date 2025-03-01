cmake_minimum_required (VERSION 3.8)

project(loc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#---------------------------------------------#

set(SRCS 

loc/loc.hpp

tests/tests.cpp
tests/picojson.h

)

#---------------------------------------------#

add_executable ("${PROJECT_NAME}" ${SRCS})

add_compile_options(/utf-8)

add_custom_command(
    OUTPUT "${CMAKE_BINARY_DIR}/data.json"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/tests/data.json" "${CMAKE_BINARY_DIR}/data.json"
    DEPENDS "${CMAKE_SOURCE_DIR}/tests/data.json"
)

add_custom_target(copy_data_json ALL DEPENDS "${CMAKE_BINARY_DIR}/data.json")