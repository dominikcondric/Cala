#include "Transformation.h"

Transformation& Transformation::translate(const glm::vec3& t)
{
	translationVector += t;
	transformMatrix = glm::scale(glm::translate(glm::mat4(1.f), translationVector) * getRotationMatrix(), scaleVector);
	return *this;
}

Transformation& Transformation::rotate(float angle, const glm::vec3& rotationAxis)
{
	rotationQuat = glm::normalize(glm::rotate(rotationQuat, glm::radians(angle), glm::normalize(rotationAxis)));
	transformMatrix = glm::scale(glm::translate(glm::mat4(1.f), translationVector) * getRotationMatrix(), scaleVector);
	return *this;
}

Transformation& Transformation::scale(const glm::vec3& s)
{
	scaleVector = glm::max(scaleVector * s, glm::vec3(0.f));
	transformMatrix = glm::scale(glm::translate(glm::mat4(1.f), translationVector) * getRotationMatrix(), scaleVector);
	return *this;
}

Transformation& Transformation::scale(float scalingFactor)
{
	return scale(glm::vec3(scalingFactor));
}