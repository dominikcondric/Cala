set(src_files
    Rendering/Camera.h              Rendering/Camera.cpp
    Rendering/ConstantBuffer.h      Rendering/ConstantBuffer.cpp
    Rendering/Framebuffer.h         Rendering/Framebuffer.cpp
    Rendering/Mesh.h                Rendering/Mesh.cpp
    Rendering/GraphicsAPI.h         Rendering/GraphicsAPI.cpp
    Rendering/Shader.h              Rendering/Shader.cpp
    Rendering/Texture.h             Rendering/Texture.cpp
    
    Rendering/Renderers/IRenderer.h
    Rendering/Renderers/LightRenderer.h         Rendering/Renderers/LightRenderer.cpp 
    Rendering/Renderers/SkyboxRenderer.h        Rendering/Renderers/SkyboxRenderer.cpp 
    Rendering/Renderers/SimpleRenderer.h   Rendering/Renderers/SimpleRenderer.cpp 
    Rendering/Renderers/HelperGridRenderer.h    Rendering/Renderers/HelperGridRenderer.cpp 
    Rendering/Renderers/PostProcessor.h         Rendering/Renderers/PostProcessor.cpp 

    Utility/Image.h                 Utility/Image.cpp
    Utility/Time.h                 Utility/Time.cpp
    Utility/Model.h                 Utility/Model.cpp
    Utility/Window.h                Utility/Window.cpp
    Utility/IOSystem.h              Utility/IOSystem.cpp
    Utility/Transformation.h        Utility/Transformation.cpp
    Utility/BaseApplication.h        Utility/BaseApplication.cpp
    Utility/Core.h                    Utility/Core.cpp

    Rendering/Shaders/HelperGridFragmentShader.glsl
    Rendering/Shaders/HelperGridGeometryShader.glsl
    Rendering/Shaders/HelperGridVertexShader.glsl
    Rendering/Shaders/GeneralVertexShader.glsl
    Rendering/Shaders/LightFragmentShader.glsl
    Rendering/Shaders/PostProcessingVertexShader.glsl
    Rendering/Shaders/PostProcessingFragmentShader.glsl
    Rendering/Shaders/SkyboxVertexShader.glsl
    Rendering/Shaders/SkyboxFragmentShader.glsl
)

# Cala library
add_library(Cala STATIC
    ${src_files}
)

target_compile_definitions(Cala PUBLIC SHADERS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/Rendering/Shaders/")

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