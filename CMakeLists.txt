cmake_minimum_required(VERSION 3.24.0)
project(blaze)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories for different configurations
foreach(OUTPUTCONFIG DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    string(TOUPPER "${OUTPUTCONFIG}" UPPERCASE_CONFIG)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${UPPERCASE_CONFIG} "${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}/lib")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${UPPERCASE_CONFIG} "${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}/bin")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${UPPERCASE_CONFIG} "${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}/bin")
endforeach()

include(FetchContent)

# Add fmt
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 11.1.4
)
FetchContent_MakeAvailable(fmt)

# Add googletest
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.16.0
)
FetchContent_MakeAvailable(googletest)

# Find ICU package
find_package(ICU REQUIRED uc data i18n)

# Gather all source files
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")

# Split main file from core code
list(FILTER SOURCE_FILES EXCLUDE REGEX ".*main\\.cpp") # Exclude main.cpp

# Core library target (only non-main files)
add_library(blaze_core STATIC ${SOURCE_FILES})

target_link_libraries(blaze_core PUBLIC fmt::fmt ICU::uc ICU::data ICU::i18n)
target_include_directories(blaze_core PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Add test directory
add_subdirectory(tests)

# Executable target (uses main.cpp and links to core)
add_executable(blaze src/main.cpp)
target_link_libraries(blaze PRIVATE blaze_core)
