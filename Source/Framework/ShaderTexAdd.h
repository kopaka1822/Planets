#pragma once
#include "Shader.h"

class ShaderTexAdd : public Shader
{
public:
	void Load()
	{
		Shader::Load("data/shader/texAdd");
	}
	virtual void Create() override
	{
		Shader::Create();

		texLocation = glGetUniformLocation(program, "tex");
		if (texLocation == -1)
			throw Exception("texadd shader: tex uniform not found");

		glUseProgram(program);
		glUniform1i(texLocation, 0);
		glUseProgram(0);

		facLocation = glGetUniformLocation(program, "factor");
		if (facLocation == -1)
			throw Exception("texadd shader: factor uniform not found");
	}
	virtual void Dispose() override
	{
		texLocation = -1;
		facLocation = -1;

		Shader::Dispose();
	}
	void SetFactor(float mul)
	{
		glUseProgram(program);
		glUniform1f(facLocation, mul);
		glUseProgram(0);
	}
private:
	GLint texLocation = -1;
	GLint facLocation = -1;
};
