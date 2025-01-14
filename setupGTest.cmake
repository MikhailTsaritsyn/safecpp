option(BUILD_GMOCK ON)
option(INSTALL_GTEST OFF)

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY "https://github.com/google/googletest"
        GIT_TAG "v1.15.2"
        GIT_PROGRESS TRUE
)

message(STATUS "Fetching Googletest...")
FetchContent_MakeAvailable(googletest)
message(STATUS "Googletest support (source)")

enable_testing()

add_executable(safecpp_test
        test/example.cpp
)

target_include_directories(safecpp_test PRIVATE test)
target_link_libraries(safecpp_test PRIVATE gtest_main)
include(GoogleTest)
gtest_discover_tests(safecpp_test)