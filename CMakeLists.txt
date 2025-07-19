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

# Create library
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_library(ParallaxDB_lib ${SOURCES})

# Link LLVM libraries
llvm_map_components_to_libnames(LLVM_LIBS core orcjit native)

target_link_libraries(ParallaxDB_lib ${LLVM_LIBS})

# Add main executable
add_executable(ParallaxDB src/main.cpp)
target_link_libraries(ParallaxDB ParallaxDB_lib)

# Add tests
enable_testing()
add_executable(ParallaxDB_tests tests/test_basic.cpp)
add_executable(ParallaxDB_parser_tests tests/test_parser.cpp)
target_link_libraries(ParallaxDB_tests ParallaxDB_lib)
target_link_libraries(ParallaxDB_parser_tests ParallaxDB_lib)
add_test(NAME BasicTests COMMAND ParallaxDB_tests)
add_test(NAME ParserTests COMMAND ParallaxDB_parser_tests)
