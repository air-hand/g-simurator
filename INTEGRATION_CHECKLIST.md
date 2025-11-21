# Test Integration Checklist

Use this checklist to ensure successful integration of the WindowKeyboard test suite.

## ✅ Pre-Integration Verification

- [x] Test files created in src/tests/
  - [x] window_keyboard_test.cpp (536 lines, 29 tests)
  - [x] window_integration_test.cpp (306 lines, 16 tests)
  - [x] main_proc_refactor_test.cpp (344 lines, 19 tests)

- [x] Documentation files created
  - [x] src/tests/README.md
  - [x] src/tests/TEST_SETUP.md
  - [x] src/tests/TEST_SUMMARY.md
  - [x] src/tests/BUILD_INTEGRATION.md
  - [x] src/tests/QUICK_REFERENCE.md
  - [x] src/tests/REGENERATE_CMAKE.md
  - [x] UNIT_TESTS_ADDED.md
  - [x] TEST_INTEGRATION_COMPLETE.md
  - [x] TEST_FILES_MANIFEST.md
  - [x] INTEGRATION_CHECKLIST.md (this file)

- [x] Test files follow project patterns
  - [x] Use `#include <gtest/gtest.h>`
  - [x] Use `import utils;` for modules
  - [x] Follow existing test naming conventions
  - [x] Match code style of existing tests

## 📋 Integration Steps

### Step 1: CMakeLists.txt Regeneration
- [ ] Navigate to src/tests directory
- [ ] Run PowerShell script: `pwsh -File .\CMakeLists.txt.ps1`
- [ ] Verify CMakeLists.txt was updated
- [ ] Check that new test files are included

**Command:**
```powershell
cd src/tests
pwsh -File .\CMakeLists.txt.ps1
```

**Expected Output:**
CMakeLists.txt should include:
- "main_proc_refactor_test.cpp"
- "window_integration_test.cpp"
- "window_keyboard_test.cpp"

### Step 2: Project Build
- [ ] Clean previous build (optional): `rm -rf build`
- [ ] Configure CMake: `cmake -B build`
- [ ] Build project: `cmake --build build`
- [ ] Verify no compilation errors
- [ ] Check that test executable was created

**Commands:**
```bash
cd ../..  # Back to repository root
cmake -B build
cmake --build build
```

**Expected Output:**
- No compilation errors
- Build succeeds
- `simurator-test` executable created in build directory

### Step 3: Test Execution
- [ ] Navigate to build directory
- [ ] Run ctest: `ctest --output-on-failure`
- [ ] Verify all new tests pass (64 tests)
- [ ] Verify existing tests still pass
- [ ] Check test execution time (should be < 2 seconds total)

**Commands:**
```bash
cd build
ctest --output-on-failure
```

**Expected Output:**