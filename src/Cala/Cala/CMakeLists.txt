set(src_files
    Rendering/Camera.h              Rendering/Camera.cpp
    Rendering/ConstantBuffer.h      Rendering/ConstantBuffer.cpp
    Rendering/Framebuffer.h         Rendering/Framebuffer.cpp
    Rendering/Mesh.h                Rendering/Mesh.cpp
    Rendering/GraphicsAPI.h         Rendering/GraphicsAPI.cpp
    Rendering/Shader.h              Rendering/Shader.cpp
    Rendering/ITexture.h            Rendering/ITexture.cpp
    Rendering/Texture.h             Rendering/Texture.cpp
    Rendering/TextureArray.h             Rendering/TextureArray.cpp
    Rendering/NativeAPI.h
    Rendering/GPUResource.h
    
    Rendering/Renderers/IRenderer.h
    Rendering/Renderers/ICameraRenderer.h
    Rendering/Renderers/LightRenderer.h         Rendering/Renderers/LightRenderer.cpp 
    Rendering/Renderers/SkyboxRenderer.h        Rendering/Renderers/SkyboxRenderer.cpp 
    Rendering/Renderers/SimpleRenderer.h   Rendering/Renderers/SimpleRenderer.cpp 
    Rendering/Renderers/HelperGridRenderer.h    Rendering/Renderers/HelperGridRenderer.cpp 
    Rendering/Renderers/PostProcessingRenderer.h         Rendering/Renderers/PostProcessingRenderer.cpp 

    Utility/Image.h                 Utility/Image.cpp
    Utility/Time.h                 Utility/Time.cpp
    Utility/Model.h                 Utility/Model.cpp
    Utility/ModelLoader.h                 Utility/ModelLoader.cpp
    Utility/GLFWWindow.h                Utility/GLFWWindow.cpp
    Utility/IWindow.h               Utility/IWindow.cpp
    Utility/IIOSystem.h              Utility/IIOSystem.cpp
    Utility/GLFWIOSystem.h              Utility/GLFWIOSystem.cpp
    Utility/Transformation.h        Utility/Transformation.cpp
    Utility/BaseApplication.h        Utility/BaseApplication.cpp
    Utility/Logger.h                Utility/Logger.cpp
    Utility/Platform.h
)

# Cala library
add_library(Cala STATIC
    ${src_files}
)

if(MSVC)
  target_compile_options(Cala PRIVATE /W4)
else()
  target_compile_options(Cala PRIVATE -Wall -Wextra -Wpedantic)
endif()


target_compile_definitions(Cala 
    PUBLIC 
        SHADERS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/Rendering/Shaders/"
        CALA_API_OPENGL
)

target_compile_features(Cala PUBLIC cxx_std_17)

set_target_properties(Cala
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib
)

target_link_libraries(Cala
    PRIVATE
        glad
        OpenGL::GL
        glfw
)

target_include_directories(Cala
    PRIVATE
        ${dependencies_dir}/ImageLoader/
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../
        ${dependencies_dir}/glm/
)