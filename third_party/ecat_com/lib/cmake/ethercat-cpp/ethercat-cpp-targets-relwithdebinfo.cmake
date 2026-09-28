#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ethercat-cpp::ethercat-cpp" for configuration "RelWithDebInfo"
set_property(TARGET ethercat-cpp::ethercat-cpp APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(ethercat-cpp::ethercat-cpp PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELWITHDEBINFO "spdlog::spdlog;Boost::filesystem"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/libethercat-cpp.so.4.1.4"
  IMPORTED_SONAME_RELWITHDEBINFO "libethercat-cpp.so.4.1.4"
  )

list(APPEND _IMPORT_CHECK_TARGETS ethercat-cpp::ethercat-cpp )
list(APPEND _IMPORT_CHECK_FILES_FOR_ethercat-cpp::ethercat-cpp "${_IMPORT_PREFIX}/lib/libethercat-cpp.so.4.1.4" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
