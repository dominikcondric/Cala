#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/matrix_float4x4.hpp>

namespace Cala {
	class Camera {
	public:
		enum class Directions { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };
		enum class Type { Orthographic, Perspective };

	public:
		Camera(Type _type);
		~Camera() = default;
		Camera(const Camera& other);
		Camera& operator=(const Camera& other);
		void setPosition(const glm::vec3& position);
		void setCenter(const glm::vec3& center);
		void setProjectionNearPlane(float near);
		void setProjectionFarPlane(float far);
		void setProjectionPlanes(float left, float right, float top, float bottom);
		void setProjectionViewingAngle(float angle);
		void setProjectionAspectRatio(float ratio);
		float getProjectionAspectRatio() const { return aspectRatio; }
		float getProjectionViewingAngle() const { return viewingAngle; }
		float getProjectionFarPlane() const { return farPlane; }
		float getProjectionNearPlane() const { return nearPlane; }
		glm::vec4 getOrthographicSidePlanes() const { return glm::vec4(topPlane, leftPlane, bottomPlane, rightPlane); }
		void rotate(const glm::vec2& offset);
		void move(Directions direction, float deltaTime);
		void setRotationSensitivity(float sensitivity);
		float getRotationSensitivity() const { return rotationSensitivity; }
		void setMoveSensitivity(float sensitivity);
		float getMoveSensitivity() const { return moveSensitivity; }
		void setType(Type _type);
		Type getType() const { return type; }
		const glm::vec3& getCenter() const { return center; }
		const glm::vec3& getPosition() const { return position; }
		const glm::vec3& getUDirection() const { return uDirection; }
		const glm::vec3& getVDirection() const { return vDirection; }
		const glm::vec3& getWDirection() const { return wDirection; }
		const glm::mat4& getView() const { return view; }
		const glm::mat4& getProjection() const { return *currentProjection; }

	protected:
		void recalculateEulerAngles();
		void perspectiveProject();
		void orthographicProject();
		Type type;
	 	const glm::mat4* currentProjection = nullptr;
		glm::vec3 position{ 0.f, 10.f, 15.f };
		glm::vec3 center{ 0.f, 0.f, 0.f };
		const glm::vec3 lookUp{ 0.f, 1.f, 0.f };
		glm::vec3 wDirection;
		glm::vec3 uDirection;
		glm::vec3 vDirection;
		glm::mat4 view;
		glm::mat4 orthographicProjection;
		glm::mat4 perspectiveProjection;
		float roll = 0.f;
		float yaw = 3 * glm::pi<float>() / 2;
		float pitch = -glm::pi<float>() / 4;
		float rotationSensitivity{ 2e-3f };
		float moveSensitivity{ 10.f };
		float nearPlane = 1.f;
		float farPlane = 100.f;
		float leftPlane = -1.f;
		float rightPlane = 1.f;
		float topPlane = 1.f;
		float bottomPlane = -1.f;
		float aspectRatio = 1024 / 768.f;
		float viewingAngle = 45.f;
	};
}