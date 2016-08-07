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
	void Update(const PointF& p)
	{
		glUseProgram(program);
		glUniform2f(rtexel, p.x, p.y);
		glUseProgram(0);
	}
	virtual void Create() override
	{
		Shader::Create();

		color = glGetUniformLocation(program, "color");
		if (color == -1)
			throw Exception("starfield shader: color uniform not found");

		rtexel = glGetUniformLocation(program, "rtexel");
		if (rtexel == -1)
			throw Exception("starfield shader: rtexel uniform not found");

		offset = glGetUniformLocation(program, "offset");
		if (offset == -1)
			throw Exception("starfield shader: offset uniform not found");

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
	GLint color = -1;
	Color curCol = -1;
	Color desCol = -1; // desired
	GLint rtexel = -1;
	GLint offset = -1;
};
