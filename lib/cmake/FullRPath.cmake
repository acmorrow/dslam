set (CMAKE_SKIP_BUILD_RPATH false)
set (CMAKE_BUILD_WITH_INSTALL_RPATH false)
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH true)

# TODO(acm): I'd really like to do this with $ORIGIN on linux, and
# whatever the equivalent is on Mac OS.
list (FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
if ("${isSystemDir}" STREQUAL "-1")
    set (CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)
    set (CMAKE_INSTALL_NAME_DIR ${CMAKE_INSTALL_PREFIX}/lib)
endif ()
