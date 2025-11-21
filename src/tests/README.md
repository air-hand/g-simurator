# Unit Tests for g-simurator

## Overview

This directory contains unit and integration tests for the g-simurator project, focusing on the WindowKeyboard functionality and Window class integration.

## Test Files

### window_keyboard_test.cpp
Comprehensive unit tests for the `WindowKeyboard` class covering:
- **Basic Operations**: Single key press/release, multiple key combinations
- **Edge Cases**: Empty vectors, null HWND, maximum WORD values
- **Modifier Keys**: Ctrl, Shift, Alt combinations in various configurations
- **Virtual Keys**: All standard Windows virtual keys (VK_*)
- **Function Keys**: F1-F12
- **Alphanumeric Keys**: A-Z, 0-9
- **Numpad Keys**: Numpad 0-9
- **Move Semantics**: Move constructor and assignment operator
- **Performance**: Rapid sequential key presses
- **Integration Scenarios**: Complete typing sequences and complex shortcuts

### window_integration_test.cpp
Integration tests for the `Window` class's `Keyboard()` method covering:
- **Factory Method**: Keyboard instance creation from Window
- **Window Handle Consistency**: Proper HWND propagation
- **Instance Independence**: Multiple keyboards for different windows
- **Resource Management**: Proper cleanup and lifecycle management
- **Thread Safety**: Conceptual thread safety validation
- **Usage Patterns**: Real-world usage scenarios from main_proc.cpp
- **Exception Safety**: Robust error handling

## Building and Running Tests

### Prerequisites
- Google Test framework (via vcpkg or system package manager)
- CMake 3.20 or higher
- C++20 compatible compiler (MSVC 2019+ on Windows)

### Build Instructions

```bash
# From project root
mkdir -p build
cd build
cmake ..
cmake --build . --target window_keyboard_test window_integration_test
```

### Running Tests

```bash
# Run all tests
ctest

# Run specific test suite
./window_keyboard_test
./window_integration_test

# Run with verbose output
ctest --verbose

# Run tests matching pattern
ctest -R "WindowKeyboard.*"
```

### Test Output

Tests use Google Test framework with the following result indicators:
- `[  PASSED  ]` - Test passed successfully
- `[  FAILED  ]` - Test failed with details
- `[ SKIPPED ]` - Test was skipped

## Test Coverage

The test suite provides comprehensive coverage for:

1. **API Contract Testing**: All public methods and interfaces
2. **Edge Case Validation**: Boundary conditions and unusual inputs
3. **Error Handling**: Invalid parameters and exceptional states
4. **Performance**: Overhead and throughput validation
5. **Integration**: Component interaction and real-world usage patterns

## Testing Philosophy

These tests follow several key principles:

- **Isolation**: Tests are independent and can run in any order
- **Clarity**: Test names clearly describe what is being tested
- **Completeness**: Cover happy paths, edge cases, and failure modes
- **Maintainability**: Easy to understand and update
- **Fast Execution**: Quick feedback during development

## Mock Objects

The tests use mock implementations to avoid dependencies on:
- Actual Windows API calls
- Real HWND handles
- Window activation/focus operations

This ensures:
- Tests run quickly
- No side effects on the system
- Deterministic behavior
- Easy debugging

## Adding New Tests

When adding new tests:

1. Follow existing naming conventions
2. Use descriptive test names (e.g., `TEST_F(ClassName, MethodName_Scenario_ExpectedBehavior)`)
3. Include setup/teardown in test fixtures
4. Document complex test scenarios
5. Ensure tests are independent
6. Add appropriate assertions

Example:
```cpp
TEST_F(WindowKeyboardTest, NewFeature_ValidInput_ReturnsExpectedResult) {
    // Arrange
    WindowKeyboardTestImpl keyboard(test_hwnd_);
    
    // Act
    keyboard.NewFeature();
    
    // Assert
    EXPECT_EQ(/* expected result */);
}
```

## Continuous Integration

These tests are designed to run in CI/CD pipelines:
- Fast execution (< 1 second total)
- No external dependencies
- Clear pass/fail indicators
- Detailed failure messages

## Known Limitations

1. **Module Testing**: The C++20 module system makes direct testing challenging. Tests use a parallel implementation approach.
2. **Windows API**: Tests mock Windows API calls rather than testing actual system behavior.
3. **UI Interaction**: No actual window creation or user input simulation.

## Future Enhancements

- Add property-based testing for key sequences
- Increase coverage for error conditions
- Add benchmarking for performance regression detection
- Integration with code coverage tools
- Add stress tests for resource limits

## Contributing

When contributing tests:
1. Ensure all existing tests pass
2. Add tests for new functionality
3. Update this README if adding new test files
4. Follow the project's coding standards
5. Keep tests focused and maintainable

## License

Tests follow the same license as the main project.