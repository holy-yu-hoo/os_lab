cmake_minimum_required(VERSION 3.14)
project(monitor LANGUAGES C CXX)

add_executable(monitor)

set(SOURCES main.cpp)

set(HEADERS )

target_sources(monitor PRIVATE ${SOURCES})