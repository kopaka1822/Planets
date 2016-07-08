#pragma once
#include "Shader.h"
#include "Color.h"

class ShaderPlanet : public Shader
{
public:
	ShaderPlanet()
	{}
	void Load()
	{
		Shader::Load("data/shader/planet");
	}
	virtual void Create() override
	{
		Shader::Create();

		// locate uniforms
		texNoise = glGetUniformLocation(program, "noise");
		if (texNoise == -1)
			throw Exception("planet shader: noise uniform not found");

		subCol = glGetUniformLocation(program, "subcol");
		if (subCol == -1)
			throw Exception("planet shader: subcol uniform not found");

		move = glGetUniformLocation(program, "texmove");
		if (move == -1)
			throw Exception("planet shader: texmove uniform not found");

		glUseProgram(program);
		glUniform1i(texNoise, 0);
		glUseProgram(0);
	}
	virtual void Dispose() override
	{
		// Dispose uniforms
		texNoise = -1;
		subCol = -1;
		move = -1;

		Shader::Dispose();
	}

	void SetParameters(Color cmain, Color csub, PointF d)
	{
		glColor4f(cmain.r, cmain.g, cmain.b, cmain.a);

		glUseProgram(program);

		glUniform4f(subCol, csub.r, csub.g, csub.b, csub.a);
		glUniform2f(move, d.x, d.y);

		glUseProgram(0);
		GLCheck("SetParameters planet");
	}
private:
	GLint texNoise = -1;
	GLint subCol = -1;
	GLint move = -1;
};
