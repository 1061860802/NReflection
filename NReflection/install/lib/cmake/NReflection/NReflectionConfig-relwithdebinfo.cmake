#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "NReflection::NReflectionCore" for configuration "RelWithDebInfo"
set_property(TARGET NReflection::NReflectionCore APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(NReflection::NReflectionCore PROPERTIES
  IMPORTED_IMPLIB_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/NReflectionCore.lib"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/bin/NReflectionCore.dll"
  )

list(APPEND _cmake_import_check_targets NReflection::NReflectionCore )
list(APPEND _cmake_import_check_files_for_NReflection::NReflectionCore "${_IMPORT_PREFIX}/lib/NReflectionCore.lib" "${_IMPORT_PREFIX}/bin/NReflectionCore.dll" )

# Import target "NReflection::NReflectionExe" for configuration "RelWithDebInfo"
set_property(TARGET NReflection::NReflectionExe APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(NReflection::NReflectionExe PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/bin/NReflectionExe.exe"
  )

list(APPEND _cmake_import_check_targets NReflection::NReflectionExe )
list(APPEND _cmake_import_check_files_for_NReflection::NReflectionExe "${_IMPORT_PREFIX}/bin/NReflectionExe.exe" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
