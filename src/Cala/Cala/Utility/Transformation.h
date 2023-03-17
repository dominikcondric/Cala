#pragma once 
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Cala {
	class Transformation {
	public:
		Transformation() = default;
		~Transformation() = default;
		Transformation& translate(const glm::vec3& t);
		Transformation& rotate(float angle, const glm::vec3& rotationAxis);
		Transformation& scale(const glm::vec3& s);
		Transformation& scale(float scalingFactor);
		const glm::mat4& getTransformMatrix() const { return transformMatrix; }
		const glm::vec3& getTranslation() const { return translationVector; }
		const glm::vec3& getScale() const { return scaleVector; }
		const glm::quat& getRotationQuat() const { return rotationQuat; }
		const glm::mat4 getRotationMatrix() const { return glm::mat4_cast(rotationQuat); }

	private:
		glm::mat4 transformMatrix{ 1.f };
		glm::quat rotationQuat{ glm::angleAxis(0.f, glm::vec3(1.f, 0.f, 0.f)) };
		glm::vec3 translationVector{ 0.f, 0.f, 0.f };
		glm::vec3 scaleVector{ 1.f, 1.f, 1.f };
	};
}