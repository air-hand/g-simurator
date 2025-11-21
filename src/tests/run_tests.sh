#!/bin/bash
# Convenience script to build and run tests

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}Building tests...${NC}"

# Create build directory if it doesn't exist
mkdir -p ../../build/tests
cd ../../build/tests

# Configure CMake
cmake ../../src/tests

# Build tests
cmake --build . --config Release

echo -e "${YELLOW}Running tests...${NC}"

# Run tests with output
if ctest --output-on-failure; then
    echo -e "${GREEN}All tests passed!${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed!${NC}"
    exit 1
fi