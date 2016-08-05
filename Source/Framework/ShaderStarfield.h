#pragma once
#include "Shader.h"

class ShaderStarfield : public Shader
{
public:
	virtual ~ShaderStarfield() override
	{

	}
	void Load()
	{
		Shader::Load("data/shader/starfield");
	}
	void setColor(const Color& col)
	{
		desCol = col;
	}

	virtual void Create() override
	{
		Shader::Create();

		color = glGetUniformLocation(program, "color");
		if (color == -1)
			throw Exception("starfield shader: color uniform not found");

		setColor(Color::White());

		curCol = Color::White();
		desCol = Color::White();
	}

	virtual void Bind() override
	{
		// update uniform
		curCol = curCol.mix(desCol, 0.2);

		glUseProgram(program);
		glUniform3f(color, curCol.r, curCol.g, curCol.b);
		glUseProgram(0);
		Shader::Bind();
	}
private:
	GLint color;
	Color curCol;
	Color desCol; // desired
};