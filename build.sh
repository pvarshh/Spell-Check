#!/bin/bash

# Build script for Spell Checker
# This script provides an easy way to build, test, and install the spell checker

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if required tools are installed
check_dependencies() {
    print_status "Checking dependencies..."
    
    if ! command -v cmake &> /dev/null; then
        print_error "CMake is not installed. Please install CMake 3.15 or later."
        exit 1
    fi
    
    if ! command -v make &> /dev/null; then
        print_error "Make is not installed. Please install Make."
        exit 1
    fi
    
    if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
        print_error "No C++ compiler found. Please install GCC or Clang."
        exit 1
    fi
    
    print_success "All dependencies are available."
}

# Clean previous builds
clean_build() {
    print_status "Cleaning previous builds..."
    rm -rf build
    mkdir -p build
    print_success "Build directory cleaned."
}

# Build the project
build_project() {
    print_status "Configuring project with CMake..."
    cd build
    
    if [ "$1" = "debug" ]; then
        cmake -DCMAKE_BUILD_TYPE=Debug ..
        print_status "Building in Debug mode..."
    else
        cmake -DCMAKE_BUILD_TYPE=Release ..
        print_status "Building in Release mode..."
    fi
    
    print_status "Compiling source code..."
    make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
    
    cd ..
    print_success "Build completed successfully!"
}

# Run basic tests
run_tests() {
    print_status "Running basic tests..."
    
    # Test 1: Help message
    print_status "Test 1: Help message"
    if ./build/spell_checker --help > /dev/null 2>&1; then
        print_success "✓ Help message test passed"
    else
        print_error "✗ Help message test failed"
        return 1
    fi
    
    # Test 2: Dictionary loading
    print_status "Test 2: Dictionary loading"
    if ./build/spell_checker --stats > /dev/null 2>&1; then
        print_success "✓ Dictionary loading test passed"
    else
        print_error "✗ Dictionary loading test failed"
        return 1
    fi
    
    # Test 3: Single word check (correct word)
    print_status "Test 3: Correct word check"
    output=$(./build/spell_checker -w "hello" 2>/dev/null)
    if echo "$output" | grep -q "is spelled correctly"; then
        print_success "✓ Correct word test passed"
    else
        print_error "✗ Correct word test failed"
        return 1
    fi
    
    # Test 4: Single word check (misspelled word)
    print_status "Test 4: Misspelled word check"
    output=$(./build/spell_checker -w "teh" 2>/dev/null)
    if echo "$output" | grep -q "Suggestions"; then
        print_success "✓ Misspelled word test passed"
    else
        print_error "✗ Misspelled word test failed"
        return 1
    fi
    
    # Test 5: File checking
    print_status "Test 5: File checking"
    if ./build/spell_checker sample_text.txt > /dev/null 2>&1; then
        print_success "✓ File checking test passed"
    else
        print_error "✗ File checking test failed"
        return 1
    fi
    
    print_success "All basic tests passed!"
}

# Install the binary
install_binary() {
    print_status "Installing spell checker..."
    
    # Check if we have write permissions
    if [ -w "/usr/local/bin" ]; then
        cp build/spell_checker /usr/local/bin/
        mkdir -p /usr/local/share/spell_checker/dictionaries
        cp dictionaries/en_US.dict /usr/local/share/spell_checker/dictionaries/
        print_success "Spell checker installed to /usr/local/bin/"
    else
        print_warning "No write permission to /usr/local/bin. Trying with sudo..."
        sudo cp build/spell_checker /usr/local/bin/
        sudo mkdir -p /usr/local/share/spell_checker/dictionaries
        sudo cp dictionaries/en_US.dict /usr/local/share/spell_checker/dictionaries/
        print_success "Spell checker installed to /usr/local/bin/ (with sudo)"
    fi
}

# Print usage information
print_usage() {
    echo "Usage: $0 [COMMAND]"
    echo ""
    echo "Commands:"
    echo "  build         Build the project in release mode (default)"
    echo "  debug         Build the project in debug mode"
    echo "  clean         Clean build directory"
    echo "  test          Run basic tests"
    echo "  install       Install the binary to /usr/local/bin"
    echo "  all           Clean, build, and test"
    echo "  help          Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0            # Same as '$0 build'"
    echo "  $0 debug     # Build in debug mode"
    echo "  $0 all       # Clean build and test everything"
}

# Main execution
main() {
    case "${1:-build}" in
        "build")
            check_dependencies
            clean_build
            build_project release
            ;;
        "debug")
            check_dependencies
            clean_build
            build_project debug
            ;;
        "clean")
            clean_build
            ;;
        "test")
            if [ ! -f "build/spell_checker" ]; then
                print_error "Binary not found. Please build first."
                exit 1
            fi
            run_tests
            ;;
        "install")
            if [ ! -f "build/spell_checker" ]; then
                print_error "Binary not found. Please build first."
                exit 1
            fi
            install_binary
            ;;
        "all")
            check_dependencies
            clean_build
            build_project release
            run_tests
            ;;
        "help"|"-h"|"--help")
            print_usage
            ;;
        *)
            print_error "Unknown command: $1"
            print_usage
            exit 1
            ;;
    esac
}

# Run main function with all arguments
main "$@"
