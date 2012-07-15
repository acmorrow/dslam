set (libdslam_version_major 0)
set (libdslam_version_minor 1)
set (libdslam_version_patch 0)

include_directories (${PROJECT_SOURCE_DIR}/src/lib)
include_directories (${PROJECT_BINARY_DIR}/src/lib)

configure_file ("config.hpp.in" "config.hpp")

file (GLOB libdslam_sources *.cpp)
file (GLOB libdslam_headers *.hpp)

add_library (libdslam ${libdslam_sources})

target_link_libraries (libdslam
  ${Pion_NET_LIBRARY} ${Pion_COMMON_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${OPENSSL_LIBRARIES})

set_target_properties (libdslam PROPERTIES
  OUTPUT_NAME dslam
  DEFINE_SYMBOL LIBDSLAM_EXPORTS
  VERSION "${libdslam_version_major}.${libdslam_version_minor}.${libdslam_version_patch}")

generate_export_header (libdslam EXPORT_FILE_NAME export.hpp)

list (APPEND libdslam_generated_headers
  ${CMAKE_CURRENT_BINARY_DIR}/export.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/config.hpp)

install (TARGETS libdslam DESTINATION lib)
install (FILES ${libdslam_headers} ${libdslam_generated_headers} DESTINATION include/dslam)

add_subdirectory(test)
