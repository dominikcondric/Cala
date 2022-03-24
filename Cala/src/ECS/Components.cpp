#include "Components.h"

void TransformComponent::translate(const glm::vec3& t)
{
	translationVector += t;
	transformMatrix = glm::scale(glm::translate(glm::mat4(1.f), translationVector) * getRotationMatrix(), scaleVector);
}

void TransformComponent::rotate(float angle, const glm::vec3& rotationAxis)
{
	rotationQuat = glm::normalize(glm::rotate(rotationQuat, glm::radians(angle), glm::normalize(rotationAxis)));
	transformMatrix = glm::scale(glm::translate(glm::mat4(1.f), translationVector) * getRotationMatrix(), scaleVector);
}

void TransformComponent::scale(const glm::vec3& s)
{
	scaleVector = glm::max(scaleVector * s, glm::vec3(0.f));
	transformMatrix = glm::scale(glm::translate(glm::mat4(1.f), translationVector) * getRotationMatrix(), scaleVector);

}