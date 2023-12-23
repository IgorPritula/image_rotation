cmake_minimum_required(VERSION 3.5)

project(ImageRotation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

FILE(COPY res DESTINATION "${CMAKE_BINARY_DIR}")

include_directories(3rdParty)

add_executable(ImageRotation
    main.cpp
    rotate.cpp
)

install(TARGETS ImageRotation
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
