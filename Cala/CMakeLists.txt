configure_file (
	src/Core/Utility/Path.h.in
	${CMAKE_CURRENT_BINARY_DIR}/src/Path.h
)

set (srcFiles
	src/Core/BaseApplication.cpp 	src/Core/BaseApplication.h
	src/Core/IOSystem.cpp 			src/Core/IOSystem.h
	src/Core/Window.cpp			 	src/Core/Window.h
	src/Core/Utility/Image.h		src/Core/Utility/Image.cpp
	src/Core/Utility/FileDialog.h	src/Core/Utility/FileDialog.cpp
	src/Core/Utility/Time.h			src/Core/Utility/Time.cpp
	src/Core/Core.h 				src/Core/Core.cpp
	src/Core/Utility/IterableVector.h
	src/Core/Utility/ConsistentVector.h
	src/Core/Utility/Path.h.in

	src/ECS/ComponentDatabase.h
	src/ECS/ConsistentComponentVector.h
	src/ECS/ComponentIterator.h
	src/ECS/SceneSerializerTemplates.h
	src/ECS/SceneSerializer.h		src/ECS/SceneSerializer.cpp
	src/ECS/Components.cpp 			src/ECS/Components.h
	src/ECS/Scene.cpp 				src/ECS/Scene.h

	src/Graphics/RenderingSystem.h	src/Graphics/RenderingSystem.cpp
	src/Graphics/Camera.h			src/Graphics/Camera.cpp
	src/Graphics/Mesh.h				src/Graphics/Mesh.cpp
	src/Graphics/GraphicsAPI.h		src/Graphics/GraphicsAPI.cpp
	src/Graphics/Texture.h
	src/Graphics/ConstantBuffer.h
	src/Graphics/Framebuffer.h
	src/Graphics/Shader.h

	src/Graphics/OpenGL/OGLConstantBuffer.cpp	src/Graphics/OpenGL/OGLConstantBuffer.h
	src/Graphics/OpenGL/OGLFramebuffer.cpp		src/Graphics/OpenGL/OGLFramebuffer.h
	src/Graphics/OpenGL/OGLMesh.cpp				src/Graphics/OpenGL/OGLMesh.h
	src/Graphics/OpenGL/OGLGraphicsAPI.cpp		src/Graphics/OpenGL/OGLGraphicsAPI.h
	src/Graphics/OpenGL/OGLShader.cpp			src/Graphics/OpenGL/OGLShader.h
	src/Graphics/OpenGL/OGLTexture.cpp			src/Graphics/OpenGL/OGLTexture.h
	src/Graphics/OpenGL/OGLErrorChecker.h

	src/GUI/ImGuiWindow.cpp		src/GUI/ImGuiWindow.h
	src/GUI/ImGuiRenderer.cpp	src/GUI/ImGuiRenderer.h

	src/Shaders/PostProcessingVertexShader.glsl
	src/Shaders/PostProcessingFragmentShader.glsl
	src/Shaders/GeneralVertexShader.glsl
	src/Shaders/GeneralFragmentShader.glsl
	src/Shaders/BaseMeshVertexShader.glsl
	src/Shaders/BaseMeshGeometryShader.glsl
	src/Shaders/BaseMeshFragmentShader.glsl
	src/Shaders/SkyboxVertexShader.glsl
	src/Shaders/SkyboxFragmentShader.glsl
)

add_library(Cala
	STATIC
		${srcFiles}
)

set_property (GLOBAL PROPERTY USE_FOLDERS ON)

if (MSVC)
	target_compile_options(Cala PUBLIC /W4 /Ot)
else()
	target_compile_options(Cala PUBLIC -Wall -Wextra -pedantic -O2)
endif()

if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.8.)
	source_group (TREE
		${CMAKE_CURRENT_SOURCE_DIR} 
		FILES 
			${srcFiles}
	)
endif()

target_link_libraries(Cala
	PUBLIC
		ImGui
	PRIVATE 
		glfw
		OpenGL::GL
		glad
)

target_include_directories(Cala
	PUBLIC
		src/
		${CMAKE_SOURCE_DIR}/Dependencies/glm
		${CMAKE_CURRENT_BINARY_DIR}/src
		${CMAKE_SOURCE_DIR}/Dependencies/RapidXML
	PRIVATE
		${CMAKE_SOURCE_DIR}/Dependencies/ImageLoader
)