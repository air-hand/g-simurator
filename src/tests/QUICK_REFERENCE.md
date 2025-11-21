# Test Suite Quick Reference

## 🚀 Quick Start

```bash
# Build and run all tests
cd src/tests
./run_tests.sh

# Or manually:
mkdir -p ../../build/tests && cd ../../build/tests
cmake ../../src/tests && cmake --build . && ctest --output-on-failure
```

## 📋 Test Files Overview

| File | Purpose | Test Count | Key Focus |
|------|---------|------------|-----------|
| `window_keyboard_test.cpp` | WindowKeyboard unit tests | 29 | Key input simulation, edge cases |
| `window_integration_test.cpp` | Window-Keyboard integration | 16 | Component interaction, lifecycle |
| `main_proc_refactor_test.cpp` | Refactoring validation | 19 | Migration correctness, patterns |

## 🎯 Common Test Commands

### Run All Tests
```bash
ctest --output-on-failure
```

### Run Specific Test Suite
```bash
./window_keyboard_test
./window_integration_test
./main_proc_refactor_test
```

### Run Filtered Tests
```bash
./window_keyboard_test --gtest_filter="*KeyDown*"
./window_integration_test --gtest_filter="*Keyboard*"
```

### Run with Verbose Output
```bash
./window_keyboard_test --gtest_verbose=all
```

### List Available Tests
```bash
./window_keyboard_test --gtest_list_tests
```

## 🔍 Test Categories

### WindowKeyboard Tests