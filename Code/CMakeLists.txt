# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)


# Set the project name
project(OrcFortress)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add a CMake option to enable or disable debug mode
option(ENABLE_DEBUG "Enable debug mode" ON)

# Set the build type based on the ENABLE_DEBUG option
if(ENABLE_DEBUG)
    message("Debug mode enabled")
    set(CMAKE_BUILD_TYPE Debug)
    add_compile_options(-g -O0)
else()
    set(CMAKE_BUILD_TYPE Release)
    add_compile_options(-O3)  # Optimization flag for Release build
endif()

# Set architecture-specific flags
if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "i686")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv8-a")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a")
endif()

# Ensure position-independent code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Specify output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib_external)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Disable building tests for libraries.
set(BUILD_TESTING OFF CACHE BOOL "Disable building tests" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "Disable building tests" FORCE)
set(BUILD_SAMPLES OFF CACHE BOOL "Disable building samples" FORCE) # Enable these if you need samples in poc's
set(BUILD_EXAMPLES OFF CACHE BOOL "Disable building examples" FORCE) # Enable these if you need examples in poc's



include_directories(${PROJECT_SOURCE_DIR}/inc)
include_directories(${PROJECT_SOURCE_DIR}/inc/FRONTEND)
include_directories(${PROJECT_SOURCE_DIR}/inc/BACKEND)
include_directories(${PROJECT_SOURCE_DIR}/external/sqlite3)
# Automatically search for source files
file(GLOB_RECURSE SOURCES
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    #${PROJECT_SOURCE_DIR}/src/FRONTEND/*.cpp,
    #${PROJECT_SOURCE_DIR}/src/BACKEND/*.cpp
)

# Add SQLite3 source file
set(SQLITE3_SOURCES ${PROJECT_SOURCE_DIR}/external/sqlite3/sqlite3.c)

# Add an executable
add_executable(OrcFortress ${SOURCES} ${SQLITE3_SOURCES} ${PROJECT_SOURCE_DIR}/src/main.cpp)

# Conditionally include headers and set compiler options based on the operating system
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_definitions(OrcFortress PRIVATE -D_WIN32)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_definitions(OrcFortress PRIVATE -D_LINUX)
endif()