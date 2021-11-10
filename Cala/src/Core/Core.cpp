#include "Core/Core.h"

#if defined CALA_PLATFORM_WINDOWS
	#include <windows.h>

namespace Cala {
	void sleep(uint64_t miliseconds)
	{
		Sleep(miliseconds);
	}
}

#elif defined CALA_PLATFORM_LINUX
	#include <unistd.h>

namespace Cala {
	void sleep(uint64_t miliseconds)
	{
		sleep(miliseconds);
	}
}

#elif defined CALA_PLATFORM_APPLE
#error "MacOS is not supported for now!"
#endif

