include_directories (${PROJECT_SOURCE_DIR}/src/lib)
include_directories (${PROJECT_BINARY_DIR}/src/lib)

add_executable (nyccpp_meetup_example nyccpp_meetup_example.cpp)
target_link_libraries (nyccpp_meetup_example
  libdslam
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
)
install (TARGETS nyccpp_meetup_example DESTINATION bin)

add_executable (minimal_example minimal_example.cpp)
target_link_libraries (minimal_example
  libdslam
)
install (TARGETS minimal_example DESTINATION bin)
