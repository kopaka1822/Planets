#pragma once
#include <string>
#include "../glew/glew.h"
#include <vector>
#include <assert.h>
#include "../glm/matrix.hpp"
#include "../Utility/GL_Exception.h"

class Shader
{
	enum SHADERTYPE
	{
		VERTEX,
		FRAGMENT,

		S_SIZE
	};
public:
	Shader();
	virtual void Load(const std::string& filename);
	virtual void Create();
	virtual void Dispose();

	virtual void Bind() const;
	void Undbind() const
	{
		glUseProgram(0);
	}

	~Shader();

private:
	static std::string LoadShader(const std::string& fileName);
	static void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);
	GLuint CreateShader(SHADERTYPE ty, GLenum shadertype);
	
	void LoadShaderSource(const std::string& filename);
protected:
	GLuint program = 0;
	GLuint shaders[S_SIZE];

	std::string sources[S_SIZE];
	std::string filename;

	bool bLoaded = false;
};