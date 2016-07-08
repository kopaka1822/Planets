#include "Shader.h"
#include <fstream>
#include "../System/Log.h"
#include "../Utility/FileException.h"
#include <cstring>

Shader::Shader()
{
	memset(shaders, 0, sizeof(shaders));
}
void Shader::Load(const std::string& filename)
{
	if (bLoaded)
		return;

	this->filename = filename;
	LoadShaderSource(filename);

	bLoaded = true;
}
void Shader::Create()
{
	assert(program == 0);
	program = glCreateProgram();
	memset(shaders, 0, sizeof(shaders));

	shaders[VERTEX] = CreateShader(VERTEX, GL_VERTEX_SHADER);
	shaders[FRAGMENT] = CreateShader(FRAGMENT, GL_FRAGMENT_SHADER);

	for (size_t i = 0; i < S_SIZE; i++)
	{
		if (shaders[i])
		{
			glAttachShader(program, shaders[i]);
		}
	}

	glLinkProgram(program);
	CheckShaderError(program, GL_LINK_STATUS, true, "programm linking failed");

	glValidateProgram(program);
	CheckShaderError(program, GL_VALIDATE_STATUS, true, "programm validation failed");
}
void Shader::Dispose()
{
	if (program)
	{
		for (size_t i = 0; i < S_SIZE; i++)
		{
			if (shaders[i])
			{
				glDetachShader(program, shaders[i]);
				glDeleteShader(shaders[i]);
				shaders[i] = 0;
			}
		}

		glDeleteProgram(program);
		program = 0;
	}
}
void Shader::LoadShaderSource(const std::string& filename)
{
	sources[VERTEX] = LoadShader(filename + ".vs");
	sources[FRAGMENT] = LoadShader(filename + ".fs");
}
void Shader::Bind() const
{
	assert(bLoaded);
	assert(program);
	glUseProgram(program);
}
std::string Shader::LoadShader(const std::string& fileName)
{
	std::ifstream file;
	file.open((fileName).c_str());

	std::string output;
	std::string line;

	if (file.is_open())
	{
		while (file.good())
		{
			getline(file, line);
			output.append(line + "\n");
		}
	}
	else
	{
		throw ExMissingFile(fileName);
	}

	return output;
}

void Shader::CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage)
{
	GLint success = 0;
	GLchar error[1024] = { 0 };

	if (isProgram)
		glGetProgramiv(shader, flag, &success);
	else
		glGetShaderiv(shader, flag, &success);

	if (success == GL_FALSE)
	{
		if (isProgram)
			glGetProgramInfoLog(shader, sizeof(error), NULL, error);
		else
			glGetShaderInfoLog(shader, sizeof(error), NULL, error);

		throw Exception((errorMessage + std::string(":\n") + error).c_str());
	}
}

GLuint Shader::CreateShader(SHADERTYPE ty, GLenum shadertype)
{
	const std::string& text = sources[ty];

	auto shader = glCreateShader(shadertype);

	if (shader == 0)
		throw Exception("shader creation failed");

	const GLchar* shaderSourceStrings[1];
	GLint shaderSourceStringsLength[1];

	shaderSourceStrings[0] = text.c_str();
	shaderSourceStringsLength[0] = text.length();

	glShaderSource(shader, 1, shaderSourceStrings, shaderSourceStringsLength);
	glCompileShader(shader);

	CheckShaderError(shader, GL_COMPILE_STATUS, false, "shader compiliation failed: " + filename);

	return shader;
}
Shader::~Shader()
{
	bLoaded = false;
}
