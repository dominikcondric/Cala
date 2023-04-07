#include "Camera.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

#define SPACE_MOVE (moveSensitivity * deltaTime)
#define FULL_CIRCLE (2 * glm::pi<float>())

namespace Cala {
	Camera::Camera(Type _type)
	{
		setType(_type);
		recalculateEulerAngles();
	}

	void Camera::recalculateEulerAngles()
	{
		wDirection = glm::normalize(center - position);
		center = position + wDirection;
		uDirection = glm::cross(wDirection, lookUp);
		vDirection = glm::cross(uDirection, wDirection);

		pitch = glm::asin(glm::dot(glm::vec3(0.f, 1.f, 0.f), wDirection));
		yaw = glm::atan(glm::dot(glm::vec3(0.f, 0.f, -1.f), wDirection) / glm::dot(glm::vec3(1.f, 0.f, 0.f), wDirection));

		if (wDirection.z < 0.f)
		{
			yaw = FULL_CIRCLE - yaw;
		}

		view = glm::lookAt(position, center, lookUp);
	}

    void Camera::setPosition(const glm::vec3 &newPosition)
    {
		position = newPosition;
		center = position + wDirection;
		view = glm::lookAt(position, center, lookUp);
	}

	void Camera::setCenter(const glm::vec3& newCenter)
	{
		center = newCenter;
		recalculateEulerAngles();
	}

	void Camera::setUp(const glm::vec3& up)
	{
		lookUp = up;
		recalculateEulerAngles();
	}

	void Camera::setProjectionNearPlane(float near)
	{
		nearPlane = near;
		perspectiveProject();
		orthographicProject();
	}

	void Camera::setProjectionFarPlane(float far)
	{
		farPlane = far;
		perspectiveProject();
		orthographicProject();
	}

    void Camera::setProjectionViewingAngle(float angle)
	{
		viewingAngle = angle;
		perspectiveProject();
	}

	void Camera::setProjectionAspectRatio(float ratio)
	{
		aspectRatio = ratio;
		perspectiveProject();
	}

	void Camera::rotateCamera(const glm::vec2& offset)
	{
		float mouseOffsetX = offset.x;
		float mouseOffsetY = offset.y;

		yaw += mouseOffsetX * mouseSensitivity;
		if (yaw < 0.f)
			yaw += FULL_CIRCLE;
		else if (yaw > FULL_CIRCLE)
			yaw -= FULL_CIRCLE;

		if (pitch + (mouseOffsetY * mouseSensitivity) < glm::pi<float>() / 2 && pitch + (mouseOffsetY * mouseSensitivity) > -glm::pi<float>() / 2)
			pitch += mouseOffsetY * mouseSensitivity;

		wDirection.x = glm::cos(yaw) * glm::cos(pitch);
		wDirection.y = glm::sin(pitch);
		wDirection.z = glm::sin(yaw) * glm::cos(pitch);
		wDirection = glm::normalize(wDirection);
		center = position + wDirection;
		uDirection = glm::cross(wDirection, lookUp);
		vDirection = glm::cross(uDirection, wDirection);

		view = glm::lookAt(position, center, lookUp);
	}

	void Camera::setMouseSensitivity(float sensitivity)
	{
		mouseSensitivity = sensitivity;
		recalculateEulerAngles();
	}

	void Camera::setMoveSensitivity(float sensitivity)
	{
		moveSensitivity = sensitivity;
		recalculateEulerAngles();
	}

	void Camera::moveCamera(Directions direction, float deltaTime)
	{
		switch (direction)
		{
			case Directions::FORWARD:
				position += wDirection * SPACE_MOVE;
				center += wDirection * SPACE_MOVE;
				break;

			case Directions::BACKWARD:
				position -= wDirection * SPACE_MOVE;
				center -= wDirection * SPACE_MOVE;
				break;

			case Directions::LEFT:
				position -= uDirection * SPACE_MOVE;
				center -= uDirection * SPACE_MOVE;
				break;

			case Directions::RIGHT:
				position += uDirection * SPACE_MOVE;
				center += uDirection * SPACE_MOVE;
				break;

			case Directions::UP:
				position += lookUp * SPACE_MOVE;
				center += lookUp * SPACE_MOVE;
				break;

			case Directions::DOWN:
				position -= lookUp * SPACE_MOVE;
				center -= lookUp * SPACE_MOVE;
				break;
		}

		view = glm::lookAt(position, center, lookUp);
	}

    void Camera::setType(Type _type)
    {
		type = _type;
		switch (type)
		{
			case Type::Orthographic:
				currentProjection = &orthographicProjection;
				break;
			case Type::Perspective:
				currentProjection = &perspectiveProjection;
				break;
		};
    }

    void Camera::perspectiveProject()
	{
		perspectiveProjection = glm::perspective(glm::radians(viewingAngle), aspectRatio, nearPlane, farPlane);
	}

	void Camera::orthographicProject()
	{
		orthographicProjection = glm::ortho(leftPlane, rightPlane, nearPlane, farPlane);
	}

    void Camera::setProjectionPlanes(float left, float right, float top, float bottom)
    {
		leftPlane = left;
		rightPlane = right;
		topPlane = top;
		bottomPlane = bottom;
		orthographicProject();
    }
}