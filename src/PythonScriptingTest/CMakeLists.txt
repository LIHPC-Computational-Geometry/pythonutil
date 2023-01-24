include (${CMAKE_SOURCE_DIR}/cmake/version.cmake)
include (${CMAKE_SOURCE_DIR}/cmake/common.cmake)
include (${CMAKE_SOURCE_DIR}/cmake/python_binding.cmake)
include (${CMAKE_SOURCE_DIR}/cmake/workarounds.cmake)

find_package (TkUtilScripting 5 REQUIRED)

file (GLOB CPP_SOURCES *.i *.cpp)

set (ALL_TARGETS PythonScriptingTest)
#set (UseSWIG_TARGET_NAME_PREFERENCE STANDARD)
set_property (SOURCE PythonScriptingTest.i PROPERTY CPLUSPLUS ON)
swig_add_library (PythonScriptingTest TYPE SHARED LANGUAGE PYTHON SOURCES ${CPP_SOURCES})
cmake_policy (SET CMP0086 NEW)
target_link_libraries (PythonScriptingTest PUBLIC PythonUtil)
target_link_libraries (PythonScriptingTest PUBLIC TkUtilScripting::TkUtilScripting)
# C++ : APRES swig_add_library
set_property (SOURCE PythonScriptingTest.i PROPERTY CPLUSPLUS ON)
set_source_files_properties (PythonScriptingTest.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties (PythonScriptingTest.i PROPERTIES SWIG_MODULE_NAME PythonScriptingTest)
# Objectif avoir les flags de compilation SWIG suivants :
# -globals PythonScriptingTest -shadow -DIN_STD="std::" -DSWIG_FORBIDS_FINALIZATION -DIN_UTIL="TkUtil::" -DIN_STD="std::"
# Ne marche pas sans le -c++ malgré les CPLUSPLUS ON avant le swig_add_library.
set (MT_SWIG_COMPILE_OPTIONS "-c++;-module;PythonScriptingTest;-globals;PythonScriptingTest;-shadow;-DIN_STD=\"std::\";-DSWIG_FORBIDS_FINALIZATION;-DIN_UTIL=\"TkUtil::\";-DIN_STD=\"std::\"")
set_source_files_properties (PythonScriptingTest.i PROPERTIES SWIG_MODULE_NAME PythonScriptingTest)
foreach (lib ${ALL_TARGETS})
if (USE_PYTHON_3)
	target_include_directories (${lib} PRIVATE ${Python3_INCLUDE_DIRS})
else (USE_PYTHON_3)
	target_include_directories (${lib} PRIVATE ${Python2_INCLUDE_DIRS})
endif (USE_PYTHON_3)
   target_compile_options (${lib} PUBLIC ${MANDATORY_CXX_OPTIONS})
   set_property (TARGET ${lib} PROPERTY VERSION ${PYTHON_UTIL_VERSION})
   set_property (TARGET ${lib} PROPERTY SOVERSION ${PYTHON_UTIL_MAJOR_VERSION})
   set_property (TARGET ${lib} PROPERTY SWIG_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/src/PythonUtil/public)
   set_property (TARGET ${lib} PROPERTY SWIG_COMPILE_OPTIONS ${MT_SWIG_COMPILE_OPTIONS})
endforeach (lib)
target_include_directories (PythonScriptingTest PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)
