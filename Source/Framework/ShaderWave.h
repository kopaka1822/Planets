#pragma once
#include "Shader.h"
#include "Color.h"

class ShaderWave : public Shader
{
public:
	ShaderWave()
	{}
	void Load()
	{
		Shader::Load("data/shader/wave");
	}
	virtual void Create() override
	{
		Shader::Create();

		// locate uniforms
		progress = glGetUniformLocation(program, "progress");
		if (progress == -1)
			throw Exception("wave shader: progress uniform not found");
	}
	virtual void Dispose() override
	{
		// Dispose uniforms
		progress = -1;

		Shader::Dispose();
	}

	void SetParameters(Color col,float perc)
	{
		glColor4f(col.r, col.g, col.b, col.a);
		glUseProgram(program);

		glUniform1f(progress, perc);

		glUseProgram(0);
		GLCheck("SetParameters planet");
	}
private:
	GLint progress = -1;
};
