# Regenerating CMakeLists.txt

This project uses a PowerShell script to automatically generate `CMakeLists.txt` from all `.cpp` files in the tests directory.

## How to Regenerate

From the repository root, run:

```powershell
# On Windows
pwsh -File src/tests/CMakeLists.txt.ps1

# Or from src/tests directory
cd src/tests
pwsh -File .\CMakeLists.txt.ps1
```

This will automatically:
1. Find all `.cpp` test files in the directory
2. Generate a `CMakeLists.txt` with all files included
3. Configure them as sources for the `${PROJECT_NAME}-test` target

## New Test Files Added

The following test files have been added for WindowKeyboard functionality:
- `window_keyboard_test.cpp` - Unit tests for WindowKeyboard class
- `window_integration_test.cpp` - Integration tests for Window::Keyboard() method
- `main_proc_refactor_test.cpp` - Tests validating the main_proc.cpp refactoring

These will be automatically included when you regenerate the CMakeLists.txt.

## Build Process

After regenerating CMakeLists.txt:

```bash
# Configure
cmake -B build

# Build
cmake --build build

# Run tests
cd build
ctest --output-on-failure
```

## Note on Test Integration

The new test files integrate with the existing test infrastructure:
- They use the same Google Test framework as existing tests
- They follow the same naming conventions (test_*.cpp pattern should be updated to match)
- They will be compiled into the existing `${PROJECT_NAME}-test` executable
- They use the same module imports (`import utils;`, etc.)

## Alternative: Manual CMakeLists.txt

If you prefer not to use the PowerShell script, you can manually update `CMakeLists.txt` to include the new source files.