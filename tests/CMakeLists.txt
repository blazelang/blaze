enable_testing()

# Collect all test source files
file(GLOB_RECURSE TEST_SOURCES "*.cpp")

# Create test executable
add_executable(blaze_tests ${TEST_SOURCES})

# Link test executable with googletest and the main project
target_link_libraries(blaze_tests PRIVATE gtest gtest_main blaze_core)

# Include headers and source dirs
target_include_directories(blaze_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
)

# Enable googletest test discovery
include(GoogleTest)
gtest_discover_tests(blaze_tests)
