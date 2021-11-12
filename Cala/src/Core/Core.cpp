#include "Core/Core.h"

#if defined CALA_PLATFORM_WINDOWS
	#include <windows.h>

namespace Cala {
	void sleep(uint64_t milliseconds)
	{
		Sleep(milliseconds);
	}
}

#elif defined CALA_PLATFORM_LINUX
	#include <time.h>

namespace Cala {
	void sleep(uint64_t milliseconds)
	{
		struct timespec ts;
		ts.tv_sec = milliseconds / 1000;
		ts.tv_nsec = (milliseconds % 1000) * 1000000;
		nanosleep(&ts, NULL);
		sleep(milliseconds);
	}
}

#elif defined CALA_PLATFORM_APPLE
#error "MacOS is not supported for now!"
#endif

