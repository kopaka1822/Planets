#pragma once
#include "../glew/glew.h"
#include <exception>
#include <string>
#include "Exception.h"

class GLEW_Exception : public Exception
{
public:
	GLEW_Exception(const std::string& i, GLenum error)
	:
	Exception(i + std::string(": ") + (const char*)glewGetErrorString(error))
	{}
};
