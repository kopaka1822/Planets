#pragma once
#include "Shader.h"
#include "Color.h"

class ShaderGlow : public Shader
{
public:
	ShaderGlow()
	{}
	void Load()
	{
		Shader::Load("data/shader/glow");
	}
	virtual void Create() override
	{
		Shader::Create();

		// locate uniforms
		texNoise = glGetUniformLocation(program, "noise");
		if (texNoise == -1)
			throw Exception("glow shader: noise uniform not found");

		move = glGetUniformLocation(program, "texmove");
		if (move == -1)
			throw Exception("glow shader: texmove uniform not found");

		brightness = glGetUniformLocation(program, "brightFactor");
		if (brightness == -1)
			throw Exception("glow shader: brightFactor uniform not found");

		glUseProgram(program);
		glUniform1i(texNoise, 0);
		glUseProgram(0);
	}
	virtual void Dispose() override
	{
		// Dispose uniforms
		texNoise = -1;
		move = -1;

		Shader::Dispose();
	}

	void SetParameters(Color cmain, PointF d)
	{
		glColor4f(cmain.r, cmain.g, cmain.b, cmain.a);

		glUseProgram(program);

		glUniform2f(move, d.x, d.y);

		if (lastGlow != Settings::GetPlanetGlowFactor())
		{
			lastGlow = Settings::GetPlanetGlowFactor();
			glUniform1f(brightness, lastGlow);
		}

		glUseProgram(0);
		GLCheck("SetParameters glow");
	}
private:
	GLint texNoise = -1;
	GLint move = -1;
	GLint brightness = -1;
	float lastGlow = -1.0f;
};
