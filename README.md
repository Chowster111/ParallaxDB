# ParallaxDB

**ParallaxDB** is a lightweight in-memory SQL engine prototype written in modern C++.  
It supports basic `SELECT`, `JOIN`, and `WHERE` clauses, with a custom parser, cost-based query planner, and both interpreted and LLVM JIT-compiled execution.

## Features

- Custom recursive descent SQL parser
- Logical and physical query plans with cost-based optimization
- LLVM-based JIT compilation for filter and arithmetic expressions
- Multithreaded execution pipeline using `pthreads` or `std::thread`
- Fully in-memory storage engine for fast prototyping

## Project Structure

```

ParallaxDB/
├── CMakeLists.txt
├── include/
│   ├── storage/
│   ├── parser/
│   ├── planner/
│   ├── executor/
│   ├── jit/
│   └── util/
├── src/
├── tests/
└── README.md

````

## Build Instructions

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build
make

# Run
./ParallaxDB
````

## Dependencies

* C++17 or higher (`std::variant`, `std::visit`)
* LLVM (for JIT compilation)
* pthreads

## License

Prototype project for educational purposes.

