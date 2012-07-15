include (GenerateExportHeader)
add_compiler_export_flags()

include (FullRPath)

find_package (Boost 1.50.0 COMPONENTS system)
include_directories (${Boost_INCLUDE_DIR})

find_package (Pion)
include_directories (${Pion_INCLUDE_DIRS})

find_package (OpenSSL)

find_package (GTest)

add_subdirectory (lib)
add_subdirectory (examples)
