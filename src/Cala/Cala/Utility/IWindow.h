#pragma once
#include <glm/vec2.hpp>
#include <string>
#include <memory>
#include "IIOSystem.h"

namespace Cala {
    class IWindow {
    public:
		struct Specification {
			Specification(const std::string& _windowName, uint32_t _width, uint32_t _height, uint32_t _sampleCount) :
				windowName(_windowName), width(_width), height(_height), sampleCount(_sampleCount) {}
			std::string windowName;
			uint32_t width;
			uint32_t height;
			uint32_t sampleCount;
		};
        
    public:
        IWindow(const Specification& specification);
        virtual ~IWindow();
        virtual void update() = 0;
        virtual bool exitTriggered() const = 0;
        virtual bool isResized() const = 0;
        virtual void* getNativeWindowPointer() const = 0;
        const glm::ivec2& getWindowSize() const { return windowSize; }
        const IIOSystem& getIO() const { return *ioSystem.get(); }

    protected:
        std::unique_ptr<IIOSystem> ioSystem;
        std::string windowName;
        glm::ivec2 windowSize;
        uint32_t sampleCount;

    private:
        static IWindow* instance;
    };
}