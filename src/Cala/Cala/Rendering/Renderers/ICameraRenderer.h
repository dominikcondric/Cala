#pragma once
#include "IRenderer.h"
#include "Cala/Rendering/Camera.h"

namespace Cala {
    class ICameraRenderer : IRenderer {
    public:
        ICameraRenderer() = default;
        virtual ~ICameraRenderer() = default;
        virtual void setupCamera(const Camera& camera) = 0;
    };
}