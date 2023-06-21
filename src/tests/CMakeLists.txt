enable_testing ( )

find_package (GUIToolkitsVariables)

include (${CMAKE_SOURCE_DIR}/cmake/version.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/common.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/workarounds.cmake)

set (ALL_EXECUTABLES hello script_gen python_runner)

find_package (TkUtilScripting)
foreach (exe ${ALL_EXECUTABLES})
   add_executable (${exe} ${exe}.cpp)
   target_compile_definitions (${exe} PRIVATE SHARED_PYTHON_TEST_PATH_SO="${CMAKE_CURRENT_BINARY_DIR}/../PythonScriptingTest")
   target_compile_definitions (${exe} PRIVATE SHARED_UTIL_PATH_SO="${TkUtilScripting_RUNTIME_DIR}")
   target_include_directories (${exe} PRIVATE ../PythonScriptingTest)
   target_link_libraries (${exe} PUBLIC PythonScriptingTest)
# INSTALL_RPATH modifie le rpath pour les libs internes au projet :
	set_target_properties (${exe} PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1 INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
endforeach (exe)


add_test(NAME hello COMMAND $<TARGET_FILE:hello> test.py) # will create a python file 
add_test(NAME python_runner COMMAND $<TARGET_FILE:python_runner> test.py) # execute the file previously created
add_test(NAME script_gen COMMAND $<TARGET_FILE:script_gen> sgtest.py)
