#pragma once
#include "Exception.h"
#include <string>
#include "../glew/glew.h"
#include "../System/Log.h"

class GL_Exception : public Exception
{
public:
	GL_Exception(const std::string& func, GLenum code)
		:
		Exception(""),
		code(code)
	{
		emes = func + std::string(" failed. code: ") + std::to_string(code);
		AppendInfo();
	}
	GLenum GetCode() const
	{
		return code;
	}
private:
	void AppendInfo()
	{
		switch (code)
		{
		case GL_INVALID_ENUM:
			emes += ": GL_INVALID_ENUM";
			break;
		case GLU_INVALID_VALUE:
			emes += ": GLU_INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			emes += ": GL_INVALID_OPERATION";
			break;
		case GL_STACK_OVERFLOW:
			emes += ": GL_STACK_OVERFLOW";
			break;
		case GL_STACK_UNDERFLOW:
			emes += ": GL_STACK_UNDERFLOW";
			break;
		case GL_OUT_OF_MEMORY:
			emes += ": GL_OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			emes += ": GL_INVALID_FRAMEBUFFER_OPERATION";
			break;
		case GL_TABLE_TOO_LARGE:
			emes += ": GL_TABLE_TOO_LARGE";
			break;
		}
		emes += " -> ";
		emes += (char*)glewGetErrorString(code);
	}
private:
	GLenum code;
};

class GL_Recreate : public GL_Exception
{
public:
	GL_Recreate()
		:
		GL_Exception("glEnd",GL_INVALID_OPERATION)
	{}
};

inline void GLCheck(const char* func)
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
#if _DEBUG
		__debugbreak();
		std::string info = GL_Exception(func, err).what();
		Log::Warning(info);
#else
		throw GL_Exception(func, err);
		//Log::Warning(std::string(GL_Exception(func, err).what()));
#endif
	}
}

inline void GLEndSafe()
{
	glEnd();
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		if (err == GL_INVALID_OPERATION)
		{
			throw GL_Recreate();
		}
		else
		{
			throw GL_Exception("glEnd", err);
		}
	}
}
