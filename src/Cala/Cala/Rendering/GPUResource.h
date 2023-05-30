#pragma once

namespace Cala {
    class GPUResource {
    public:
        GPUResource() = default;
        virtual ~GPUResource() = default;
        virtual void free() = 0;
        virtual bool isLoaded() const = 0;
    };
}