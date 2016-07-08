#pragma once
#include "Shader.h"
#include "../Utility/Point.h"

class ShaderStory : public Shader
{
public:
	void Load()
	{
		Shader::Load("data/shader/story");
	}
	virtual void Create() override
	{
		Shader::Create();

		bgLocation = glGetUniformLocation(program, "bg");
		if (bgLocation == -1)
			throw Exception("story shader: bg uniform not found");

		paperLocation = glGetUniformLocation(program, "txPaper");
		if (paperLocation == -1)
			throw Exception("story shader: txPaper uniform not found");

		stepLocation = glGetUniformLocation(program, "step");
		if (stepLocation == -1)
			throw Exception("story shader: step uniform not found");

		glUseProgram(program);
		glUniform1i(bgLocation, 0);
		glUniform1i(paperLocation, 1);
		glUseProgram(0);
	}
	virtual void Dispose() override
	{
		bgLocation = -1;
		paperLocation = -1;
		stepLocation = -1;

		Shader::Dispose();
	}
	void SetStep(PointF step)
	{
		glUseProgram(program);
		glUniform2f(stepLocation, step.x, step.y);
		glUseProgram(0);
	}
private:
	GLint bgLocation = -1;
	GLint paperLocation = -1;
	GLint stepLocation = -1;
};