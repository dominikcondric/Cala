#include "Camera.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/random.hpp>

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

		// Check if lookUp and direction vectors are parallel
		if (glm::abs(wDirection.x) < 1e-7 && glm::abs(wDirection.z) < 1e-7)
			wDirection.z = 0.001f;

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

    Camera::Camera(const Camera &other)
    {
		*this = other;
    }

    Camera &Camera::operator=(const Camera &other)
    {
		type = other.type;
		position = other.position;
		center = other.center;
		uDirection = other.uDirection;
		vDirection = other.vDirection;
		wDirection = other.wDirection;
		view = other.view;
		perspectiveProjection = other.perspectiveProjection;
		orthographicProjection = other.orthographicProjection;
		roll = other.roll;
		yaw = other.yaw;
		pitch = other.pitch;
		rotationSensitivity = other.rotationSensitivity;
		moveSensitivity = other.moveSensitivity;
		nearPlane = other.nearPlane;
		farPlane = other.farPlane;
		leftPlane = other.leftPlane;
		rightPlane = other.rightPlane;
		topPlane = other.topPlane;
		bottomPlane = other.bottomPlane;
		aspectRatio = other.aspectRatio;
		viewingAngle = other.viewingAngle;

		switch (type)
		{
			case Type::Orthographic:
				currentProjection = &orthographicProjection;
				break;
			case Type::Perspective:
				currentProjection = &perspectiveProjection;
				break;
		}

		return *this;
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

	void Camera::rotate(const glm::vec2& offset)
	{
		yaw += offset.x * rotationSensitivity;
		if (yaw < 0.f)
			yaw += FULL_CIRCLE;
		else if (yaw > FULL_CIRCLE)
			yaw -= FULL_CIRCLE;

		if (pitch + (offset.y * rotationSensitivity) < glm::pi<float>() / 2 && pitch + (offset.y * rotationSensitivity) > -glm::pi<float>() / 2)
			pitch += offset.y * rotationSensitivity;

		wDirection.x = glm::cos(yaw) * glm::cos(pitch);
		wDirection.y = glm::sin(pitch);
		wDirection.z = glm::sin(yaw) * glm::cos(pitch);
		wDirection = glm::normalize(wDirection);
		center = position + wDirection;

		uDirection = glm::cross(wDirection, lookUp);
		vDirection = glm::cross(uDirection, wDirection);

		view = glm::lookAt(position, center, lookUp);
	}

	void Camera::setRotationSensitivity(float sensitivity)
	{
		rotationSensitivity = sensitivity;
		recalculateEulerAngles();
	}

	void Camera::move(Directions direction, float deltaTime)
	{
		const float spaceMoveFactor = deltaTime * moveSensitivity;

		switch (direction)
		{
			case Directions::FORWARD:
				position += wDirection * spaceMoveFactor;
				center += wDirection * spaceMoveFactor;
				break;

			case Directions::BACKWARD:
				position -= wDirection * spaceMoveFactor;
				center -= wDirection * spaceMoveFactor;
				break;

			case Directions::LEFT:
				position -= uDirection * spaceMoveFactor;
				center -= uDirection * spaceMoveFactor;
				break;

			case Directions::RIGHT:
				position += uDirection * spaceMoveFactor;
				center += uDirection * spaceMoveFactor;
				break;

			case Directions::UP:
				position += lookUp * spaceMoveFactor;
				center += lookUp * spaceMoveFactor;
				break;

			case Directions::DOWN:
				position -= lookUp * spaceMoveFactor;
				center -= lookUp * spaceMoveFactor;
				break;
		}

		view = glm::lookAt(position, center, lookUp);
	}

		void Camera::setMoveSensitivity(float sensitivity)
	{
		moveSensitivity = sensitivity;
		recalculateEulerAngles();
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