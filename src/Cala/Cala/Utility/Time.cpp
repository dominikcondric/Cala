#include "Time.h"
#include <GLFW/glfw3.h>

void Cala::Time::update()
{
	lastRunningTime = runningTime;
	runningTime = glfwGetTime();
	lastDeltaTime = deltaTime;
	deltaTime = float(runningTime - lastRunningTime);
	frameRate = 1.f / deltaTime;
}

void Cala::Time::incrementalUpdate()
{
	runningTime = glfwGetTime();
	deltaTime = runningTime - lastRunningTime;
	frameRate = 1.f / deltaTime;
}
