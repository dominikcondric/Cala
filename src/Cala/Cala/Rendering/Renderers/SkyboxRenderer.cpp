#include "SkyboxRenderer.h"

namespace Cala {
	SkyboxRenderer::SkyboxRenderer(Texture* tex) : texture(tex)
	{
		mesh.loadMesh(Model().loadCube(), false);
		std::filesystem::path shadersDir(SHADERS_DIR);
		shader.attachShader(Shader::ShaderType::VertexShader, shadersDir / "SkyboxVertexShader.glsl");
		shader.attachShader(Shader::ShaderType::FragmentShader, shadersDir / "SkyboxFragmentShader.glsl");
		shader.createProgram();

		mvpBuffer.setData(shader.getConstantBufferInfo("MVP"), false);
		skyboxBlurBuffer.setData(shader.getConstantBufferInfo("SkyboxBlur"), false);
		int blurLevel = 100;
		skyboxBlurBuffer.updateData("blurValue", &blurLevel, sizeof(int));
	}

	void SkyboxRenderer::render(GraphicsAPI* const api, const Camera& camera)
	{
		if (texture == nullptr) {
			return;
		}

		api->enableSetting(GraphicsAPI::DepthTesting);
		api->setDepthComparisonFunction(GraphicsAPI::LessOrEqual);
		shader.activate();
		mvpBuffer.updateData("view", &camera.getView()[0][0], sizeof(glm::mat4));
		mvpBuffer.updateData("projection", &camera.getProjection()[0][0], sizeof(glm::mat4));
		texture->setForSampling(0);
		api->disableSetting(GraphicsAPI::FaceCulling);
		api->render(mesh);
	}
}