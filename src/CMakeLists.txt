enable_testing ( )

add_subdirectory (PythonUtil)
if (BUILD_TESTS)
	add_subdirectory (PythonScriptingTest)
	add_subdirectory (tests)
endif ( )
