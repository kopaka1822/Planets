#pragma once
#include <string>
#include "../glew/glew.h"
#include <memory>

class Texture
{
public:
	void Load(const std::string& fileName);
	void Create();
	void Dispose();
	~Texture();
	void Bind(unsigned int id) const;
	void Unbind() const;
	int GetWidth() const
	{
		return width;
	}
	int GetHeight() const
	{
		return height;
	}
private:
	GLuint tex = 0;
	int width, height;
	std::unique_ptr<char[]> pData;
};