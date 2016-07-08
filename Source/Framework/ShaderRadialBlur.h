#pragma once
#include "Shader.h"
#include "../Utility/Point.h"

class ShaderRadialBlur : public Shader
{
public:
	void Load()
	{
		Shader::Load("data/shader/radial");
	}
	virtual void Create() override
	{
		Shader::Create();

		texLocation = glGetUniformLocation(program, "tex");
		if (texLocation == -1)
			throw Exception("radial shader: tex uniform not found");

		glUseProgram(program);
		glUniform1i(texLocation, 0);
		glUseProgram(0);

		radSize = glGetUniformLocation(program, "radialSize");
		if (radSize == -1)
			throw Exception("radial shader: radialSize uniform not found");

		radBlur = glGetUniformLocation(program, "radialBlur");
		if (radBlur == -1)
			throw Exception("radial shader: radialBlur uniform not found");

		radBright = glGetUniformLocation(program, "radialBright");
		if (radBright == -1)
			throw Exception("radial shader: radialBright uniform not found");

		radOrigin = glGetUniformLocation(program, "radialOrigin");
		if (radOrigin == -1)
			throw Exception("radial shader: radialOrigin uniform not found");
	}
	virtual void Dispose() override
	{
		texLocation = -1;
		radSize = -1; // texel size
		radBlur = -1;
		radBright = -1;
		radOrigin = -1;

		Shader::Dispose();
	}
	void SetParameters(PointF origin, PointF texelSize, float blur, float brightness)
	{
		glUseProgram(program);
		glUniform2f(radOrigin, origin.x, origin.y);
		glUniform2f(radSize, texelSize.x, texelSize.y);
		glUniform1f(radBlur, blur);
		glUniform1f(radBright, brightness);
		glUseProgram(0);
	}
private:
	GLint texLocation = -1;
	GLint radSize = -1; // texel size
	GLint radBlur = -1;
	GLint radBright = -1;
	GLint radOrigin = -1;
};
