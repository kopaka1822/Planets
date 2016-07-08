#pragma once
#include "Shader.h"
#include "../Utility/Point.h"
#include "Color.h"

class ShaderDisk : public Shader
{
public:
	ShaderDisk()
	{}
	void Load()
	{
		Shader::Load("data/shader/disk");
	}
	virtual void Create() override
	{
		Shader::Create();

		colLocation = glGetUniformLocation(program, "color");
		if (colLocation == -1)
			throw Exception("disk shader: color uniform not found");
	}
	virtual void Dispose() override
	{
		colLocation = -1;

		Shader::Dispose();
	}
	void SetParameters(Color col)
	{
		this->col = col;

		glUseProgram(program);
		glUniform4f(colLocation, col.r, col.g, col.b, col.a);
		glUseProgram(0);

		GLCheck("SetParameters");
	}
private:
	GLint colLocation = -1;

	Color col;
};
