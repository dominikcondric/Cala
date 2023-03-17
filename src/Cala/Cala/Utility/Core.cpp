#include "Core.h"
 #include <chrono>
#include <thread>

void Cala::sleep(uint32_t miliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(miliseconds));
}