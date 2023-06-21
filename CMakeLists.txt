cmake_minimum_required (VERSION 3.14)

project (PythonUtil CXX)

include (GNUInstallDirs)

find_package (GUIToolkitsVariables)

include (cmake/version.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/common.cmake)

set (PythonUtil_VERSION ${PYTHON_UTIL_VERSION})

set (CMAKE_VERBOSE_MAKEFILE ON)
option (BUILD_SHARED_LIBS "Creation de bibliotheques dynamiques (defaut : ON)" ON)

enable_testing ( )

add_subdirectory (src)
