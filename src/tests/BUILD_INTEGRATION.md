# Build System Integration Guide

## Integrating Tests with Existing Build System

This guide helps you integrate the test suite into your existing CMake-based build system.

## Prerequisites

### 1. Add Google Test to vcpkg.json

Update your `vcpkg.json` to include Google Test:

```json
{
  "name": "g-simurator",
  "version-string": "1.0.0",
  "dependencies": [
    "gtest",
    "boost-program-options",
    "tesseract",
    "opencv4"
  ]
}
```

### 2. Install Dependencies

```bash
vcpkg install
```

## Main CMakeLists.txt Integration

### Option 1: Add to Existing CMakeLists.txt.ps1

Add these lines to your `CMakeLists.txt.ps1`:

```cmake
# After your main executable configuration

# Enable testing
option(BUILD_TESTING "Build the testing tree" ON)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(src/tests)
endif()
```

### Option 2: Create Separate Test Configuration

If you prefer to keep tests separate:

```cmake
# In CMakeLists.txt.ps1, add:
option(BUILD_TESTING "Build tests" OFF)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(src/tests)
endif()
```

Then build with:
```bash
cmake -DBUILD_TESTING=ON ..
```

## PowerShell Build Script Integration

### Update CMakeLists.txt.ps1 Structure

Since your project uses a `.ps1` file as CMakeLists, ensure it generates proper CMake syntax:

```powershell
# Example structure for CMakeLists.txt.ps1
@"
cmake_minimum_required(VERSION 3.20)
project(g-simurator)

# ... existing configuration ...

# Testing configuration
option(BUILD_TESTING "Build tests" ON)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(src/tests)
endif()

# ... rest of configuration ...
"@ | Out-File -FilePath CMakeLists.txt -Encoding UTF8
```

## Build Commands

### Windows (PowerShell)

```powershell
# Full build with tests
mkdir build -Force
cd build
cmake ..
cmake --build . --config Release
ctest -C Release --output-on-failure
```

### Windows (CMD)

```cmd
mkdir build
cd build
cmake ..
cmake --build . --config Release
ctest -C Release --output-on-failure
```

### Using Makefile

Update your `Makefile`:

```makefile
.PHONY: all test clean

all:
	mkdir -p build
	cd build && cmake .. && cmake --build .

test: all
	cd build && ctest --output-on-failure

clean:
	rm -rf build
```

Then run:
```bash
make test
```

## Visual Studio Integration

### 1. Generate Visual Studio Solution

```powershell
cmake -G "Visual Studio 17 2022" -A x64 ..
```

### 2. Open Solution

```powershell
start g-simurator.sln
```

### 3. Run Tests

- Tests will appear in Test Explorer (View > Test Explorer)
- Click "Run All" to execute all tests
- Individual tests can be run by right-clicking

## CI/CD Integration

### GitHub Actions

Create `.github/workflows/tests.yml`:

```yaml
name: Unit Tests

on:
  push:
    branches: [ main, fix-focus ]
  pull_request:
    branches: [ main ]

jobs:
  test:
    runs-on: windows-latest
    
    steps:
      - uses: actions/checkout@v3
      
      - name: Setup vcpkg
        uses: lukka/run-vcpkg@v10
        with:
          vcpkgDirectory: '${{ github.workspace }}/vcpkg'
          vcpkgGitCommitId: 'your-vcpkg-commit-id'
      
      - name: Install dependencies
        run: vcpkg install
      
      - name: Configure CMake
        run: cmake -B build -DBUILD_TESTING=ON
      
      - name: Build
        run: cmake --build build --config Release
      
      - name: Run tests
        run: |
          cd build
          ctest -C Release --output-on-failure
      
      - name: Upload test results
        if: always()
        uses: actions/upload-artifact@v3
        with:
          name: test-results
          path: build/Testing/Temporary/LastTest.log
```

## CMake Presets Integration

Update `CMakePresets.json`:

```json
{
  "version": 3,
  "configurePresets": [
    {
      "name": "windows-base",
      "hidden": true,
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/${presetName}",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Release"
      }
    },
    {
      "name": "windows-release",
      "inherits": "windows-base",
      "displayName": "Windows Release",
      "cacheVariables": {
        "BUILD_TESTING": "OFF"
      }
    },
    {
      "name": "windows-test",
      "inherits": "windows-base",
      "displayName": "Windows with Tests",
      "cacheVariables": {
        "BUILD_TESTING": "ON"
      }
    }
  ],
  "buildPresets": [
    {
      "name": "windows-release",
      "configurePreset": "windows-release"
    },
    {
      "name": "windows-test",
      "configurePreset": "windows-test"
    }
  ],
  "testPresets": [
    {
      "name": "windows-test",
      "configurePreset": "windows-test",
      "output": {
        "outputOnFailure": true
      }
    }
  ]
}
```

Then use:
```bash
cmake --preset windows-test
cmake --build --preset windows-test
ctest --preset windows-test
```

## Troubleshooting

### Issue: Google Test Not Found

**Symptoms:**