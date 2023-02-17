#pragma once
#include <glad/glad.h>
#include <iostream>

GLenum glCheckError_(const char* file, int line) {
	GLenum errorCode = GL_NO_ERROR;
	while ((errorCode == glGetError()) != GL_NO_ERROR)
	{
		const char* error = nullptr;
		switch (errorCode)
		{
			case GL_INVALID_ENUM: 
				error = "INVALID ENUM";
				break;
			case GL_INVALID_VALUE:
				error = "INVALID VALUE";
				break;
			case GL_INVALID_OPERATION:
				error = "INVALID OPERATION";
				break;
			case GL_OUT_OF_MEMORY:
				error = "OUT OF MEMORY";
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				error = "INVALID FRAMEBUFFER OPERATION";
				break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}

	return errorCode;
}

#define glCheckError() glCheckError_(__FILE__, __LINE__)
