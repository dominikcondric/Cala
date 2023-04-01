#pragma once
#include <glm/common.hpp>

namespace Cala {
	class Time {
	public:
		void update();
		void incrementalUpdate();
		float getDeltaTime() const { return deltaTime; }
		double getRunningTime() const { return runningTime; }
		float getFrameRate() const { return frameRate; }

	private:
		double runningTime = 0.l;
		float deltaTime = 0.f;
		float frameRate = 0.f;
		double lastRunningTime = 0.f;
		float lastDeltaTime = 0.f;
	};
}
