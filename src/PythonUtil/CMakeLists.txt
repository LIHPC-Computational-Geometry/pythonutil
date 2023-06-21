set (CURRENT_PACKAGE_NAME "PythonUtil")
set (CURRENT_PACKAGE_MACRO_NAME "PYTHON_UTIL")

find_package (GUIToolkitsVariables)

include (${CMAKE_SOURCE_DIR}/cmake/version.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/common.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/python_binding.cmake)
include (${GUIToolkitsVariables_CMAKE_DIR}/workarounds.cmake)

find_package (TkUtil 6 REQUIRED)

file (GLOB HEADERS public/${CURRENT_PACKAGE_NAME}/*.h)
file (GLOB CPP_SOURCES *.cpp)

# On ajoute les en-têtes aux sources. C'est utile pour cmake dans certains cas,
# par exemple lorsqu'ils doivent être pré-processés (moc, ...).
set (ALL_TARGETS PythonUtil)
set (PYTHON_CXX_OPTIONS -DPYTHON_VERSION="${Python_VERSION}")
set (SWIG_CXX_OPTIONS -DSWIG_FORBIDS_FINALIZATION -DSWIG_VERSION="${SWIG_VERSION}")
set (PYTHON_UTIL_CXX_OPTIONS -DTK_PYTHON_UTIL_VERSION="${PYTHON_UTIL_VERSION}")

add_library (PythonUtil ${CPP_SOURCES} ${HEADERS})
foreach (lib ${ALL_TARGETS})
   target_compile_options (${lib} PUBLIC ${MANDATORY_CXX_OPTIONS} ${PYTHON_CXX_OPTIONS})
   target_compile_options (${lib} PRIVATE ${PYTHON_UTIL_CXX_OPTIONS} ${SWIG_CXX_OPTIONS})
   target_compile_definitions (${lib} PRIVATE TOSTR_USES_CPP_IMPLEMENTATION)
   set_property (TARGET ${lib} PROPERTY VERSION ${PYTHON_UTIL_VERSION})
   set_property (TARGET ${lib} PROPERTY SOVERSION ${PYTHON_UTIL_MAJOR_VERSION})
# Etre capable une fois installée de retrouver les libs utilisées (iconv ?), ... :
   set_target_properties (${lib} PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)
# Vieux compilos/architectures (RH6, Bull, gcc 4.*)
   target_compile_definitions (${lib} PRIVATE WITHOUT_THREAD_LOCALE)
endforeach (lib)
target_include_directories (PythonUtil PRIVATE ${Python_INCLUDE_DIRS})
target_link_libraries (PythonUtil PUBLIC TkUtil::TkUtil ${Python_LIBRARIES})
target_include_directories (PythonUtil PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/public>$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)

# INSTALLATION :
include(CMakePackageConfigHelpers)
# Génération du fichier contenant les informations sur la distribution :
configure_file (cmake/PythonUtilDistribution.h.in  ${CMAKE_CURRENT_BINARY_DIR}/PythonUtilDistribution.tmp)
configure_file (${CMAKE_CURRENT_BINARY_DIR}/PythonUtilDistribution.tmp ${CMAKE_CURRENT_BINARY_DIR}/PythonUtilDistribution.h)


# ConfigPackageLocation : c'est plus ou moins standardisé, le défaut étant lib/cmake.
# On nos recommande très vivement d'utiliser ce répertoire afin de limiter les soucis par la suite,
# notamment au niveau des vues.
set (ConfigPackageLocation ${CMAKE_CMAKE_DIR})

# La génération des règles d'installation :
install (TARGETS ${ALL_TARGETS} EXPORT ${CMAKE_PACKAGE_TARGETS_NAME}
         ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
         LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
         RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
         INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Recopie des en-têtes dans le répertoire d'installation :
install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CURRENT_PACKAGE_NAME})
# Dont PythonUtilDistribution.h généré plus haut :
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/PythonUtilDistribution.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CURRENT_PACKAGE_NAME})

# Le fichier contenant les informations de version de ce module :
write_basic_package_version_file(${CMAKE_PACKAGE_VERSION_FILE} VERSION ${PYTHON_UTIL_VERSION} COMPATIBILITY SameMajorVersion)
export (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME} FILE ${CMAKE_PACKAGE_TARGETS_FILE} NAMESPACE ${CMAKE_MODULE_NAMESPACE})

# Utilisation par d'autres modules de PythonUtil :
set (INCLUDE_INSTALL_DIR include/)		# PythonUtil_INCLUDE_DIR avec PythonUtilConfig.cmake.in
set (LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})	# PythonUtil_LIB_DIR avec PythonUtilConfig.cmake.in
#set (RUNTIME_INSTALL_DIR bin/)			# PythonUtil_RUNTIME_DIR avec PythonUtilConfig.cmake.in
# Création du fichier PythonUtilConfig.cmake qui définira PythonUtil_INCLUDE_DIR, PythonUtil_LIB_DIR et PythonUtil_BIN_DIR :
configure_package_config_file (cmake/${CMAKE_PACKAGE_CONFIG_NAME}.cmake.in ${CMAKE_PACKAGE_CONFIG_FILE}
	INSTALL_DESTINATION ${CMAKE_CMAKE_DIR}
	PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR)

# Copie du fichier PythonUtilTargets.cmake dans prefix/lib/PythonUtil/cmake :
install (EXPORT ${CMAKE_PACKAGE_TARGETS_NAME}
	FILE ${CMAKE_PACKAGE_TARGETS_BASENAME} NAMESPACE ${CMAKE_MODULE_NAMESPACE}
	DESTINATION ${ConfigPackageLocation} PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE WORLD_READ)
# Copie des fichiers PythonUtilConfig.cmake et PythonUtilConfigVersion.cmake dans prefix/lib/PythonUtil/cmake :
install (FILES ${CMAKE_PACKAGE_CONFIG_FILE} ${CMAKE_PACKAGE_VERSION_FILE} DESTINATION ${ConfigPackageLocation} PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE WORLD_READ)

