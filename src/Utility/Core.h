#pragma once
#include <iostream>
#include <memory>
#include <vector>

#define LOG(message) std::cout << message << '\n'
#define LOG_ERROR(message) std::cerr << message << '\n'
#define BIT(x) (1 << x)

#if defined (__WIN32__) || defined (__CYGWIN32__) || defined (_WIN32) || defined (__MINGW__)
	#define CALA_PLATFORM_WINDOWS
#elif defined (__linux__)
	#define CALA_PLATFORM_LINUX
#elif defined (__APPLE__)
	#define CALA_PLATFORM_APPLE
#endif

template<typename T> using Unique = std::unique_ptr<T>;
template<typename T> using Shared = std::shared_ptr<T>;