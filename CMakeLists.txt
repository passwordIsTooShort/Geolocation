cmake_minimum_required(VERSION 3.22)

project(geolocationProvider LANGUAGES CXX)

option(WITH_TESTS "Enable unit tests" ON)

add_subdirectory(app)

if (WITH_TESTS)
    add_subdirectory(tests)
endif()