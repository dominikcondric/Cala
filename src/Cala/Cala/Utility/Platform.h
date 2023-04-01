#pragma once

#if defined (__WIN32__) || defined (__CYGWIN32__) || defined (_WIN32) || defined (__MINGW__)
	#define CALA_PLATFORM_WINDOWS
#elif defined (__linux__)
	#define CALA_PLATFORM_LINUX
#elif defined (__APPLE__)
	#define CALA_PLATFORM_APPLE
#endif