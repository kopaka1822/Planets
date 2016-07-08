#pragma once
#include "Shader.h"

class ShaderBrightFilter : public Shader
{
public:
	void Load()
	{
		Shader::Load("data/shader/brightFilter");
	}
	virtual void Create() override
	{
		Shader::Create();

		texLocation = glGetUniformLocation(program, "tex");
		if (texLocation == -1)
			throw Exception("cool shader: tex uniform not found");

		glUseProgram(program);
		glUniform1i(texLocation, 0);
		glUseProgram(0);
	}
	virtual void Dispose() override
	{
		texLocation = -1;

		Shader::Dispose();
	}
private:
	GLint texLocation = -1;
};
