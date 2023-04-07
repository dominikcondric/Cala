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
		void setPosition(const glm::vec3& position);
		void setCenter(const glm::vec3& center);
		void setUp(const glm::vec3& up);
		void setProjectionNearPlane(float near);
		void setProjectionFarPlane(float far);
		void setProjectionPlanes(float left, float right, float top, float bottom);
		void setProjectionViewingAngle(float angle);
		void setProjectionAspectRatio(float ratio);
		void rotateCamera(const glm::vec2& offset);
		void setMouseSensitivity(float sensitivity);
		void setMoveSensitivity(float sensitivity);
		void moveCamera(Directions direction, float deltaTime);
		void setType(Type _type);
		const glm::vec3& getPosition() const { return position; }
		const glm::vec3& getUDirection() const { return uDirection; }
		const glm::vec3& getVDirection() const { return vDirection; }
		const glm::vec3& getWDirection() const { return wDirection; }
		const glm::mat4& getView() const { return view; }
		const glm::mat4& getProjection() const { return *currentProjection; }

	protected:
	 	const glm::mat4* currentProjection = nullptr;
		void recalculateEulerAngles();
		void perspectiveProject();
		void orthographicProject();
		Type type;
		glm::vec3 position{ 0.f, 10.f, 15.f };
		glm::vec3 center{ 0.f, 0.f, 0.f };
		glm::vec3 lookUp{ 0.f, 1.f, 0.f };
		glm::vec3 wDirection;
		glm::vec3 uDirection;
		glm::vec3 vDirection;
		glm::mat4 view;
		glm::mat4 orthographicProjection;
		glm::mat4 perspectiveProjection;
		float roll = 0.f;
		float yaw = 3 * glm::pi<float>() / 2;
		float pitch = -glm::pi<float>() / 4;
		float mouseSensitivity{ 2e-3f };
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