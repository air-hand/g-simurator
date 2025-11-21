# Test Setup and Configuration Guide

## Setting Up Google Test

### Option 1: Using vcpkg (Recommended)

If your project already uses vcpkg, add Google Test to `vcpkg.json`:

```json
{
  "dependencies": [
    "gtest"
  ]
}
```

Then install:
```bash
vcpkg install
```

### Option 2: Manual Installation

#### Windows (MSVC)

```powershell
# Clone Google Test
git clone https://github.com/google/googletest.git
cd googletest
mkdir build
cd build

# Configure and build
cmake .. -DCMAKE_INSTALL_PREFIX="C:/Program Files/googletest"
cmake --build . --config Release
cmake --install . --config Release
```

#### Linux

```bash
# Ubuntu/Debian
sudo apt-get install libgtest-dev

# Build from source
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo cp lib/*.a /usr/lib
```

## Integrating with Main CMakeLists.txt

Add this to your main `CMakeLists.txt`:

```cmake
# Enable testing
enable_testing()

# Add test subdirectory
add_subdirectory(src/tests)
```

## Project-Specific Configuration

### Windows-Specific Settings

The tests assume a Windows environment with:
- `HWND` type from Windows.h
- Windows message constants (`WM_KEYDOWN`, `WM_KEYUP`)
- Virtual key codes (`VK_*`)

### C++20 Module Compatibility

Since the actual implementation uses C++20 modules, the tests:
1. Use a parallel implementation approach
2. Test the interface contract
3. Validate behavior without direct module imports

To test actual module code:
1. Wait for better C++20 module testing support in CMake/GTest
2. Use integration tests instead of unit tests
3. Consider creating a non-module test interface

## Running Tests

### From Build Directory

```bash
cd build
ctest

# With verbose output
ctest --verbose

# Run specific test
./src/tests/window_keyboard_test
```

### From Test Directory

```bash
cd src/tests
./run_tests.sh
```

### With CMake Test Target

```bash
cmake --build . --target test
```

## Test Configuration Options

### CTest Configuration

Create `CTestTestfile.cmake` in the test directory:

```cmake
# Run tests in parallel
set(CTEST_PARALLEL_LEVEL 4)

# Timeout for each test
set(CTEST_TEST_TIMEOUT 30)
```

### Google Test Options

Runtime options for Google Test:

```bash
# Run only matching tests
./window_keyboard_test --gtest_filter="*KeyDown*"

# Repeat tests
./window_keyboard_test --gtest_repeat=100

# Shuffle test order
./window_keyboard_test --gtest_shuffle

# Generate XML output
./window_keyboard_test --gtest_output=xml:test_results.xml
```

## IDE Integration

### Visual Studio

1. Open the solution
2. Go to Test > Test Explorer
3. Tests will appear automatically if Google Test Adapter is installed
4. Click "Run All" to execute tests

### Visual Studio Code

Install extensions:
- C/C++ Extension Pack
- CMake Tools
- Google Test Adapter

Configure `.vscode/settings.json`:
```json
{
    "cmake.configureSettings": {
        "ENABLE_TESTING": "ON"
    },
    "testExplorer.useNativeTesting": true
}
```

### CLion

CLion has built-in Google Test support:
1. Tests appear in the Run/Debug Configurations
2. Right-click on test file > Run
3. Use Test Explorer for navigation

## Troubleshooting

### Issue: Google Test not found

**Solution**: Ensure Google Test is in CMAKE_PREFIX_PATH:
```bash
cmake -DCMAKE_PREFIX_PATH=/path/to/gtest ..
```

### Issue: Undefined reference to Windows API

**Solution**: Link against Windows libraries:
```cmake
target_link_libraries(test_target PRIVATE user32)
```

### Issue: C++20 features not available

**Solution**: Ensure compiler supports C++20:
```cmake
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

### Issue: Tests fail in CI but pass locally

**Solution**: Check for:
- Platform-specific assumptions
- Race conditions
- Uninitialized variables
- Mock setup order

## Continuous Integration

### GitHub Actions Example

```yaml
name: Tests

on: [push, pull_request]

jobs:
  test:
    runs-on: windows-latest
    steps:
      - uses: actions/checkout@v2
      - name: Install dependencies
        run: vcpkg install gtest
      - name: Build
        run: |
          mkdir build
          cd build
          cmake ..
          cmake --build .
      - name: Test
        run: |
          cd build
          ctest --output-on-failure
```

## Code Coverage

### Using gcov/lcov (Linux)

```bash
# Build with coverage flags
cmake -DCMAKE_CXX_FLAGS="--coverage" ..
cmake --build .

# Run tests
ctest

# Generate coverage report
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_report
```

### Using Visual Studio Code Coverage

1. Enable code coverage in project properties
2. Run tests with coverage
3. View results in Test Explorer

## Performance Testing

For performance-critical code:

```cpp
TEST_F(PerformanceTest, KeyboardPerformance) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Perform operations
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    EXPECT_LT(duration.count(), threshold);
}
```

## Memory Testing

### Valgrind (Linux)

```bash
valgrind --leak-check=full ./window_keyboard_test
```

### AddressSanitizer

```cmake
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
```

## Best Practices

1. **Keep tests fast**: Unit tests should complete in milliseconds
2. **Use fixtures**: Share setup/teardown code
3. **Test one thing**: Each test should verify one behavior
4. **Avoid dependencies**: Tests should not depend on each other
5. **Use mocks**: Isolate the code under test
6. **Clear naming**: Test names should describe the scenario
7. **Assertions**: Use appropriate assertions (EXPECT vs ASSERT)
8. **Documentation**: Comment complex test scenarios

## Additional Resources

- [Google Test Primer](https://google.github.io/googletest/primer.html)
- [Google Mock Documentation](https://google.github.io/googletest/gmock_for_dummies.html)
- [CMake Testing](https://cmake.org/cmake/help/latest/manual/ctest.1.html)
- [C++20 Modules](https://en.cppreference.com/w/cpp/language/modules)