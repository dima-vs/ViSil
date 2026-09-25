cmake_minimum_required(VERSION 3.13)

project(ViSil_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_subdirectory(core)

add_executable(core_tests
    ${CORE_TEST_SOURCES}
)

target_link_libraries(core_tests
    PRIVATE
        #GTest::gtest
        #GTest::gmock
        GTest::gtest_main
        Core_Lib
)

include(GoogleTest)
gtest_discover_tests(core_tests)
