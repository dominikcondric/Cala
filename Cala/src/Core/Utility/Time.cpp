#include "Time.h"
#include <GLFW/glfw3.h>

void Cala::Time::update()
{
	lastTime = runningTime;
	runningTime = glfwGetTime();
	lastDeltaTime = deltaTime;
	deltaTime = float(runningTime - lastTime);
	frameRate = 1.f / deltaTime;
}