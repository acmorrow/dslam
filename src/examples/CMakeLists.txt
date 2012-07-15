include_directories (${PROJECT_SOURCE_DIR}/src/lib)
include_directories (${PROJECT_BINARY_DIR}/src/lib)

add_executable (nyccpp_meetup_example nyccpp_meetup_example.cpp)
target_link_libraries (nyccpp_meetup_example libdslam)
install (TARGETS nyccpp_meetup_example DESTINATION bin)
