set (editorFiles
	src/Editor/Editor.cpp 	src/Editor/Editor.h
	src/Editor/EntryPoint.cpp
)

add_executable(Calion
	${editorFiles}
)

if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.8.)
	source_group (TREE
		${CMAKE_CURRENT_SOURCE_DIR} 
		FILES 
			${editorFiles}
	)
endif()

target_link_libraries(Calion
	PRIVATE 
		Cala
)

target_include_directories(Calion
	PRIVATE
		src/
)