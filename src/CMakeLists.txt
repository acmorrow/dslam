cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

# Enforce the C++ standard, and disable extensions
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()

set(CMAKE_CXX_EXTENSIONS OFF)

include (GenerateExportHeader)
add_compiler_export_flags()

include (FullRPath)

find_package (Boost 1.50.0 COMPONENTS system program_options)
include_directories (${Boost_INCLUDE_DIR})

find_package (libpion REQUIRED)
include_directories (${LIBPION_INCLUDE_DIRS})
link_directories(${LIBPION_LIBRARY_DIRS})

find_package (OpenSSL)

find_package (GTest)

add_subdirectory (lib)
add_subdirectory (examples)
