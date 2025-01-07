cmake_minimum_required(VERSION 3.22)

set(LIBRARY_NAME geolocationProviderLib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# TODO: Add public headres (after cleanup of project structure)

set(SOURCE_FILES
    OnlineLocationProvider.cpp
    SQLiteDatabase.cpp
    IpFromUrlProvider.cpp
    BasicLocationManager.cpp
    MainWindow.cpp
)

add_library(${LIBRARY_NAME} ${SOURCE_FILES})

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

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} ${LIBRARY_NAME})
