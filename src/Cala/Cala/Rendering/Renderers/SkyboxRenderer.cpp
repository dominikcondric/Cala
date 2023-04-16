#include "SkyboxRenderer.h"
#include "Cala/Utility/Logger.h"

namespace Cala {
	SkyboxRenderer::SkyboxRenderer(const Texture* _texture)
	{
		mesh.loadFromModel(Model().loadCube(), false, false);
		mesh.culled = false;
		std::filesystem::path shadersDir(SHADERS_DIR);
		shader.attachShader(Shader::ShaderType::VertexShader, shadersDir / "SkyboxVertexShader.glsl");
		shader.attachShader(Shader::ShaderType::FragmentShader, shadersDir / "SkyboxFragmentShader.glsl");
		shader.createProgram();

		mvpBuffer.setData(shader.getConstantBufferInfo("MVP"), false);
		skyboxBlurBuffer.setData(shader.getConstantBufferInfo("SkyboxBlur"), false);
		uint32_t blurLevel = 100;
		setBlurLevel(blurLevel);

		setTexture(_texture);
	}

	void SkyboxRenderer::render(GraphicsAPI* const api, const Framebuffer* renderingTarget)
	{
		if (texture == nullptr) {
			return;
		}

		api->activateFramebuffer(renderingTarget);
		api->enableSetting(GraphicsAPI::DepthTesting);
		api->setDepthComparisonFunction(GraphicsAPI::LessOrEqual);
		shader.activate();
		texture->setForSampling(0);
		api->render(mesh);
	}

    void SkyboxRenderer::setupCamera(const Camera &camera)
    {
		mvpBuffer.updateData("view", &camera.getView()[0][0], sizeof(glm::mat4));
		mvpBuffer.updateData("projection", &camera.getProjection()[0][0], sizeof(glm::mat4));
    }

    void SkyboxRenderer::setTexture(const Texture *_texture)
    {
		if (_texture == nullptr)
			return;

		if (_texture->getDimensionality() != ITexture::Dimensionality::Cubemap)
		{
			Logger::getInstance().logErrorToConsole("Texture is not of cubemap type");
			return;
		}

		texture = _texture;
    }

    void SkyboxRenderer::setBlurLevel(uint32_t blur)
    {
		blur = glm::clamp(blur, 10U, 705U);
		skyboxBlurBuffer.updateData("blurValue", &blur, sizeof(uint32_t));
    }
}