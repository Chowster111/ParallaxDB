cmake_minimum_required(VERSION 3.15)
project(ParallaxDB)

# Set C++ version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find LLVM
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Your include directories
include_directories(include)

# Add executable
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(ParallaxDB ${SOURCES})

# Link LLVM libraries
llvm_map_components_to_libnames(LLVM_LIBS core orcjit native)

target_link_libraries(ParallaxDB ${LLVM_LIBS})

# Add tests
enable_testing()
add_executable(ParallaxDB_tests tests/test_basic.cpp)
target_link_libraries(ParallaxDB_tests ${LLVM_LIBS})
add_test(NAME BasicTests COMMAND ParallaxDB_tests)
