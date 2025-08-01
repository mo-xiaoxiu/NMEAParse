#!/bin/bash

# Default build options
BUILD_SHARED_LIBS=ON
BUILD_STATIC_LIBS=ON
BUILD_EXAMPLES=OFF
BUILD_TESTS=OFF
INSTALL_PREFIX=""
BUILD_DIR=build

# Show help information
show_help() {
    echo "NMEAParser Build Script"
    echo ""
    echo "Usage: $0 [options]"
    echo ""
    echo "Options:"
    echo "  -h, --help              Show this help message"
    echo "  --shared-only           Build shared libraries only"
    echo "  --static-only           Build static libraries only"
    echo "  --examples              Build example programs"
    echo "  --tests                 Build test programs"
    echo "  --prefix=PATH           Set installation path (default: /usr/local)"
    echo "  --build-dir=DIR         Set build directory (default: build)"
    echo "  --clean                 Clean build directory"
    echo ""
    echo "Examples:"
    echo "  $0                      # Build all library types"
    echo "  $0 --shared-only        # Build shared libraries only"
    echo "  $0 --static-only        # Build static libraries only"
    echo "  $0 --examples           # Build all libraries and examples"
    echo "  $0 --prefix=/opt/local  # Install to custom path"
    echo ""
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        --shared-only)
            BUILD_SHARED_LIBS=ON
            BUILD_STATIC_LIBS=OFF
            shift
            ;;
        --static-only)
            BUILD_SHARED_LIBS=OFF
            BUILD_STATIC_LIBS=ON
            shift
            ;;
        --examples)
            BUILD_EXAMPLES=ON
            shift
            ;;
        --tests)
            BUILD_TESTS=ON
            shift
            ;;
        --prefix=*)
            INSTALL_PREFIX="${1#*=}"
            shift
            ;;
        --build-dir=*)
            BUILD_DIR="${1#*=}"
            shift
            ;;
        --clean)
            if [ -d "$BUILD_DIR" ]; then
                echo "Cleaning build directory: $BUILD_DIR"
                rm -rf "$BUILD_DIR"
            fi
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

echo "=== NMEAParser Build Configuration ==="
echo "Build directory: $BUILD_DIR"
echo "Shared libraries: $BUILD_SHARED_LIBS"
echo "Static libraries: $BUILD_STATIC_LIBS"
echo "Example programs: $BUILD_EXAMPLES"
echo "Test programs: $BUILD_TESTS"
if [ -n "$INSTALL_PREFIX" ]; then
    echo "Install path: $INSTALL_PREFIX"
else
    echo "Install path: default (/usr/local)"
fi
echo "====================================="

# Check if CMake is installed
if ! command -v cmake &> /dev/null; then
    echo "Error: cmake not found, please install CMake first"
    echo "Ubuntu/Debian: sudo apt-get install cmake"
    echo "CentOS/RHEL: sudo yum install cmake"
    exit 1
fi

# Check if compiler supports C++17
if ! g++ -std=c++17 -x c++ -E - < /dev/null &> /dev/null; then
    echo "Error: Compiler does not support C++17, please upgrade to GCC 7.0+ or Clang 5.0+"
    exit 1
fi

# Create or clean build directory
if [ -d "$BUILD_DIR" ]; then
    echo "Cleaning build directory: $BUILD_DIR"
    rm -rf "$BUILD_DIR"
fi

echo "Creating build directory: $BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure CMake
CMAKE_ARGS="-DBUILD_SHARED_LIBS=$BUILD_SHARED_LIBS"
CMAKE_ARGS="$CMAKE_ARGS -DBUILD_STATIC_LIBS=$BUILD_STATIC_LIBS"
CMAKE_ARGS="$CMAKE_ARGS -DBUILD_EXAMPLES=$BUILD_EXAMPLES"
CMAKE_ARGS="$CMAKE_ARGS -DBUILD_TESTS=$BUILD_TESTS"

if [ -n "$INSTALL_PREFIX" ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX"
fi

echo "Configuring CMake..."
echo "cmake .. $CMAKE_ARGS"
cmake .. $CMAKE_ARGS

if [ $? -ne 0 ]; then
    echo "Error: CMake configuration failed"
    exit 1
fi

# Compile
echo "Compiling project..."
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo "Error: Compilation failed"
    exit 1
fi

# Show build results
echo ""
echo "=== Build Complete ==="
echo "Build directory: $(pwd)"
if [ "$BUILD_SHARED_LIBS" = "ON" ]; then
    if [ -f "lib/libNMEAParser.so" ]; then
        echo "Shared library: lib/libNMEAParser.so"
    fi
fi
if [ "$BUILD_STATIC_LIBS" = "ON" ]; then
    if [ -f "lib/libNMEAParser.a" ]; then
        echo "Static library: lib/libNMEAParser.a"
    fi
fi
if [ "$BUILD_EXAMPLES" = "ON" ]; then
    if [ -f "bin/nmea_example" ]; then
        echo "Example program: bin/nmea_example"
    fi
fi
if [ -f "NMEAParser.pc" ]; then
    echo "pkg-config file: NMEAParser.pc"
fi

# Ask for installation
echo ""
read -p "Install to system? (y/N): " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo "Installing to system..."
    if [ -n "$INSTALL_PREFIX" ]; then
        make install
    else
        sudo make install
    fi
    
    if [ $? -eq 0 ]; then
        echo "Installation successful!"
        echo ""
        echo "Usage examples:"
        echo "  # Using pkg-config"
        echo "  g++ -o my_program my_program.cpp \$(pkg-config --cflags --libs NMEAParser)"
        echo ""
        echo "  # Using CMake"
        echo "  find_package(NMEAParser REQUIRED)"
        echo "  target_link_libraries(your_target NMEAParser::NMEAParser_shared)"
    else
        echo "Installation failed!"
        exit 1
    fi
else
    echo "Skipping installation."
    echo ""
    echo "Manual install commands:"
    if [ -n "$INSTALL_PREFIX" ]; then
        echo "  make install"
    else
        echo "  sudo make install"
    fi
fi

echo ""
echo "Build completed!"
