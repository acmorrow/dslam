# - Try to find Pion
# Once done this will define
#  PION_FOUND - System has Pion
#  PION_INCLUDE_DIRS - The Pion include directories
#  PION_LIBRARY_DIRS - The pion library directories
#  PION_LIBRARIES - The libraries needed to use Pion

find_package (PkgConfig)
pkg_check_modules (PC_LIBPION QUIET libpion-common)
set (PION_DEFINITIONS ${PC_LIBPION_CFLAGS_OTHER})

find_path (Pion_INCLUDE_DIRS pion/PionConfig.hpp
  HINTS ${PC_LIBPION_INCLUDEDIR} ${PC_LIBPION_INCLUDE_DIRS}
  PATH_SUFFIXES pion )

find_library (Pion_COMMON_LIBRARY NAMES pion-common
  HINTS ${PC_LIBPION_LIBDIR} ${PC_LIBPION_LIBRARY_DIRS} )

find_library (Pion_NET_LIBRARY NAMES pion-net
  HINTS ${PC_LIBPION_LIBDIR} ${PC_LIBPION_LIBRARY_DIRS} )

set (Pion_COMMON_LIBRARY ${PION_COMMON_LIBRARY})
set (Pion_NET_LIBRARY ${PION_NET_LIBRARY})

set (Pion_LIBRARIES ${Pion_NET_LIBRARY} ${Pion_COMMON_LIBRARY})
foreach(_pion_my_lib ${Pion_LIBRARIES})
  get_filename_component(_pion_my_lib_path "${_pion_my_lib}" PATH)
  list(APPEND Pion_LIBRARY_DIRS ${_pion_my_lib_path})
endforeach()
list(REMOVE_DUPLICATES Pion_LIBRARY_DIRS)

include (FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set PION_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args (Pion DEFAULT_MSG
  Pion_LIBRARIES
  Pion_INCLUDE_DIRS
  Pion_LIBRARY_DIRS
  Pion_COMMON_LIBRARY
  Pion_NET_LIBRARY)

mark_as_advanced (Pion_COMMON_LIBRARY Pion_NET_LIBRARY)
