include (${CMAKE_SOURCE_DIR}/cmake/version.cmake)
include (${CMAKE_SOURCE_DIR}/cmake/common.cmake)
include (${CMAKE_SOURCE_DIR}/cmake/workarounds.cmake)

set (ALL_EXECUTABLES hello script_gen python_runner)

find_package (TkUtilScripting)
foreach (exe ${ALL_EXECUTABLES})
   add_executable (${exe} ${exe}.cpp)
   target_compile_definitions (${exe} PRIVATE SHARED_PYTHON_TEST_PATH_SO="${CMAKE_CURRENT_BINARY_DIR}/../PythonScriptingTest")
   target_compile_definitions (${exe} PRIVATE SHARED_UTIL_PATH_SO="${TkUtilScripting_RUNTIME_DIR}")
   target_include_directories (${exe} PRIVATE ../PythonScriptingTest)
   target_link_libraries (${exe} PUBLIC PythonScriptingTest)
# INSTALL_RPATH modifie le rpath pour les libs internes au projet :
	set_target_properties (${exe} PROPERTIES 
		INSTALL_RPATH_USE_LINK_PATH 1
		INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
endforeach (exe)


