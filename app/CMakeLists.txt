cmake_minimum_required(VERSION 3.22)

set(LIBRARY_NAME geolocationProviderLib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# TODO: Add public headres (after cleanup of project structure)

set(SOURCE_FILES
    src/OnlineLocationProvider.cpp
    src/SQLiteDatabase.cpp
    src/IpFromUrlProvider.cpp
    src/BasicLocationManager.cpp
    src/MainWindow.cpp
    src/Application.cpp
    network/NetworkReply.cpp
    network/NetworkAccessHandler.cpp
)

add_library(${LIBRARY_NAME} ${SOURCE_FILES})

target_include_directories(${LIBRARY_NAME}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/app
        ${CMAKE_SOURCE_DIR}/app/include
        ${CMAKE_SOURCE_DIR}/app/intf
        ${CMAKE_SOURCE_DIR}/app/network
        ${CMAKE_SOURCE_DIR}/app/src
)

target_compile_options(${LIBRARY_NAME}
    PUBLIC
        -Wall
        -Wpedantic
        -Werror
)

find_package(Qt6 REQUIRED
    COMPONENTS
        Network
        Widgets
        Core
        Sql
)

target_link_libraries(${LIBRARY_NAME}
    PUBLIC
        Qt6::Core
        Qt6::Network
        Qt6::Sql
        Qt6::Widgets
)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} ${LIBRARY_NAME})
