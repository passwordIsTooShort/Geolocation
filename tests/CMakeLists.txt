cmake_minimum_required(VERSION 3.22)

enable_testing()
find_package(GTest 1.15.2 REQUIRED)
include(GoogleTest)

include_directories(
    ${CMAKE_SOURCE_DIR}/app
    ${CMAKE_SOURCE_DIR}/tests
    ${CMAKE_SOURCE_DIR}/tests/ut
    ${CMAKE_SOURCE_DIR}/tests/mocks
    ${GTEST_INCLUDE_DIR}
)

set(LIBRARIES_FOR_TESTS
    ${GTEST_BOTH_LIBRARIES}
    GTest::gmock
    geolocationProviderLib
)

add_executable(OnlineLocationProviderTest ut/OnlineLocationProviderTest.cpp)
add_executable(BasicLocationManagerTest ut/BasicLocationManagerTest.cpp)

target_link_libraries(OnlineLocationProviderTest ${LIBRARIES_FOR_TESTS})
target_link_libraries(BasicLocationManagerTest ${LIBRARIES_FOR_TESTS})

add_test(AllTestsInOnlineLocationProvider OnlineLocationProviderTest)
add_test(AllTestsInBasicLocationManager BasicLocationManagerTest)
