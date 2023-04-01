#pragma once
#include <stdint.h>

#ifdef CALA_API_OPENGL
    using GLuint = uint32_t;
    using GLenum = uint32_t;
    #define API_NULL 0
#else
    #error "Api not supported yet!"
#endif