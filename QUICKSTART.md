# ParallaxDB Quick Start Guide

## Prerequisites

- **macOS** (tested on macOS 14+)
- **Homebrew** for package management
- **C++17** compatible compiler (Apple Clang 17+)
- **LLVM 20+** for JIT compilation

## Installation

### 1. Install Dependencies

```bash
# Install LLVM
brew install llvm

# Verify installation
export PATH="/opt/homebrew/opt/llvm/bin:$PATH"
llvm-config --version
```

### 2. Build ParallaxDB

```bash
# Clone and navigate to project
cd ParallaxDB

# Create build directory
mkdir build && cd build

# Configure with CMake
export PATH="/opt/homebrew/opt/llvm/bin:$PATH"
cmake .. -DLLVM_DIR=/opt/homebrew/opt/llvm/lib/cmake/llvm

# Build
make -j4

# Run tests
./ParallaxDB_tests
```

## Usage

### Interactive Mode

```bash
./ParallaxDB
```

Example session:
```
Welcome to ParallaxDB!

> SELECT * FROM users
1 Alice 30 
2 Bob 25 
3 Charlie 35 
4 Diana 40 

> SELECT * FROM users WHERE age > 30
3 Charlie 35 
4 Diana 40 

> exit
```

### Supported Queries

Currently supported SQL features:

- **SELECT * FROM table** - Full table scan
- **SELECT * FROM table WHERE condition** - Filtered scan
- **WHERE age > 30** - Simple numeric comparisons

### Sample Data

The database comes pre-loaded with a `users` table:

| id | name    | age |
|----|---------|-----|
| 1  | Alice   | 30  |
| 2  | Bob     | 25  |
| 3  | Charlie | 35  |
| 4  | Diana   | 40  |

## Development

### Project Structure

```
ParallaxDB/
├── include/
│   ├── storage/     # Table and data storage
│   ├── parser/      # SQL parsing
│   └── planner/     # Query planning
├── src/
│   └── main.cpp     # Main application
├── tests/
│   └── test_basic.cpp
├── CMakeLists.txt
└── README.md
```

### Adding Features

1. **New SQL features**: Extend `include/parser/Parser.hpp`
2. **New storage features**: Extend `include/storage/Table.hpp`
3. **New query plans**: Add to `include/planner/`
4. **Tests**: Add to `tests/` directory

### Building After Changes

```bash
cd build
make -j4
```

### Running Tests

```bash
./ParallaxDB_tests
```

## Next Steps

See [ROADMAP.md](ROADMAP.md) for detailed development plans.

### Immediate Priorities

1. **Real SQL parser** - Replace string matching with proper tokenization
2. **Column selection** - Support `SELECT id, name FROM users`
3. **INSERT statements** - Allow data insertion
4. **Multiple tables** - Support JOINs

### Contributing

1. Fork the repository
2. Create a feature branch
3. Add tests for new functionality
4. Submit a pull request

## Troubleshooting

### Build Issues

**LLVM not found:**
```bash
export PATH="/opt/homebrew/opt/llvm/bin:$PATH"
export LLVM_DIR="/opt/homebrew/opt/llvm/lib/cmake/llvm"
```

**Compiler errors:**
- Ensure you're using C++17 or higher
- Check that all dependencies are installed

### Runtime Issues

**Infinite loop in queries:**
- This was fixed in recent versions
- Ensure you're using the latest code

**Memory issues:**
- The database is currently in-memory only
- Large datasets may consume significant RAM

## Performance

Current performance characteristics:
- **Simple queries**: ~1000 rows/second
- **WHERE clauses**: ~500 rows/second
- **Memory usage**: ~1KB per 1000 rows

Target performance (with optimizations):
- **JIT compiled**: ~1,000,000 rows/second
- **Optimized storage**: ~100,000 rows/second 