cmake_minimum_required(VERSION 3.22)

project(geolocationProvider LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_executable(${PROJECT_NAME} main.cpp)

target_compile_options(${PROJECT_NAME}
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

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt6::Core
        Qt6::Network
        Qt6::Sql
        Qt6::Widgets
)

target_sources(${PROJECT_NAME}
    PRIVATE
        OnlineLocationProvider.cpp
        SQLiteDatabase.cpp
        IpFromUrlProvider.cpp
        BasicLocationManager.cpp
        MainWindow.cpp
)
