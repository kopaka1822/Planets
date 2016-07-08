#pragma once
#include "Shader.h"
#include "../Utility/Point.h"

class ShaderFXAA : public Shader
{
public:
	void Load()
	{
		Shader::Load("data/shader/fxaa");
	}
	virtual void Create() override
	{
		Shader::Create();

		bgLocation = glGetUniformLocation(program, "bg");
		if (bgLocation == -1)
			throw Exception("fxaa shader: bg uniform not found");

		stepLocation = glGetUniformLocation(program, "step");
		if (stepLocation == -1)
			throw Exception("fxaa shader: step uniform not found");

		spanMax = glGetUniformLocation(program, "FXAA_SPAN_MAX");
		if (spanMax == -1)
			throw Exception("fxaa shader: FXAA_SPAN_MAX uniform not found");

		reduceMul = glGetUniformLocation(program, "FXAA_REDUCE_MUL");
		if (reduceMul == -1)
			throw Exception("fxaa shader: FXAA_REDUCE_MUL uniform not found");

		subpixShift = glGetUniformLocation(program, "FXAA_SUBPIX_SHIFT");
		if (subpixShift == -1)
			throw Exception("fxaa shader: FXAA_SUBPIX_SHIFT uniform not found");

		glUseProgram(program);
		glUniform1i(bgLocation, 0);
		glUseProgram(0);
	}
	virtual void Dispose() override
	{
		bgLocation = -1;
		stepLocation = -1;

		Shader::Dispose();
	}
	void SetStep(PointF step)
	{
		glUseProgram(program);
		glUniform2f(stepLocation, step.x, step.y);

		// set uniforms
		glUniform1f(spanMax, Settings::GetFXAASpan());
		glUniform1f(reduceMul, Settings::GetFXAAMul());
		glUniform1f(subpixShift, Settings::GetFXAAShift());
		glUseProgram(0);
	}
private:
	GLint bgLocation = -1;
	GLint stepLocation = -1;

	GLint spanMax = -1;
	GLint reduceMul = -1;
	GLint subpixShift = -1;
};
