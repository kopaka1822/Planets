#pragma once
#include "Shader.h"
#include "Color.h"

class ShaderCircle : public Shader
{
public:
	ShaderCircle()
	{}
	void Load()
	{
		Shader::Load("data/shader/circle");
	}
	virtual void Create() override
	{
		Shader::Create();

		colLocation = glGetUniformLocation(program, "color");
		if (colLocation == -1)
			throw Exception("circle shader: color uniform not found");

		minradLocation = glGetUniformLocation(program, "minrad");
		if (minradLocation == -1)
			throw Exception("circle shader: minrad uniform not found");
	}
	virtual void Dispose() override
	{
		colLocation = -1;
		minradLocation = -1;

		Shader::Dispose();
	}
	void SetParameters(Color col, float minrad)
	{
		glUseProgram(program);
		glUniform4f(colLocation, col.r, col.g, col.b, col.a);
		glUniform1f(minradLocation, minrad);
		glUseProgram(0);

		GLCheck("SetParameters circle");
	}
private:
	GLint colLocation = -1;
	GLint minradLocation = -1;
};
