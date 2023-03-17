#pragma once
#include <glm/common.hpp>

namespace Cala {
	struct Time {
		double runningTime = 0.l;
		float deltaTime = 0.f;
		float frameRate = 0.f;
		double lastTime = 0.f;
		float lastDeltaTime = 0.f;

		void update();
		void incrementalUpdate();
	};
}
