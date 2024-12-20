cmake_minimum_required(VERSION 3.22)

project(geolocationProvider LANGUAGES CXX)

add_executable(${PROJECT_NAME} main.cpp)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        CMAKE_CXX_STANDARD 23
        CMAKE_CXX_STANDARD_REQUIRED YES
)

find_package(Qt6 REQUIRED COMPONENTS Network)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Sql)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Network)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Sql)

target_compile_options(${PROJECT_NAME}
    PUBLIC
        -Wall
        -Wpedantic
        -Werror
)

target_sources(${PROJECT_NAME}
    PRIVATE
        OnlineLocationProvider.cpp
        SQLiteDatabase.cpp
)