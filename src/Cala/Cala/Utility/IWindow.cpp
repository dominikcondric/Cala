#include "IWindow.h"

namespace Cala {
    IWindow* IWindow::instance = nullptr;

    IWindow::IWindow(const Specification &specification) : 
        windowName(specification.windowName), windowSize({ specification.width, specification.height }),
        sampleCount(specification.sampleCount)
    {
        assert(!instance);
        instance = this;
    }

    IWindow::~IWindow()
    {
        instance = nullptr;
    }
}