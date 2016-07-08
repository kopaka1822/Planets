#pragma once
#include "Shader.h"

class ShaderCool : public Shader
{
public:
	ShaderCool()
	{}
	void Load()
	{
		Shader::Load("data/shader/cool");
	}
	virtual void Create() override
	{
		Shader::Create();

		texLocation = glGetUniformLocation(program, "tex");
		if (texLocation == -1)
			throw Exception("cool shader: tex uniform not found");

		stepLocation = glGetUniformLocation(program, "uStep");
		if (stepLocation == -1)
			throw Exception("cool shader: uStep uniform not found");

		glUseProgram(program);
		glUniform1i(texLocation, 0);
		glUseProgram(0);
	}
	virtual void Dispose() override
	{
		texLocation = -1;
		stepLocation = -1;

		Shader::Dispose();
	}
	void SetParameters(PointF step)
	{
		glUseProgram(program);
		glUniform2f(stepLocation, step.x, step.y);
		glUseProgram(0);
	}
private:
	GLint texLocation = -1;
	GLint stepLocation = -1;
};
