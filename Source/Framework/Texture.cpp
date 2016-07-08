#include "Texture.h"
#include "stb_image.h"
#include <assert.h>
#include "../Utility/GL_Exception.h"
#include "../Utility/FileException.h"
#include <cstring>

void Texture::Load(const std::string& name)
{
	int numComponents;
	char* data = (char*)stbi_load(name.c_str(), &width, &height, &numComponents, 4);

	if (!data)
		throw ExMissingFile(name);

	// copy data
	pData = std::unique_ptr<char[]>(new char[width * height * 4]);
	memcpy(pData.get(), data, width * height * 4);

	stbi_image_free(data);
}
void Texture::Create()
{
	assert(tex == 0);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData.get());

	glGenerateMipmap(GL_TEXTURE_2D);
}
void Texture::Dispose()
{
	if (tex != 0)
	{
		glDeleteTextures(1, &tex);
		tex = 0;
	}
}
void Texture::Bind(unsigned int id) const
{
	assert(id <= 31);

	glActiveTexture(GL_TEXTURE0 + id);
	glBindTexture(GL_TEXTURE_2D, tex);
	GLCheck("glBindTexture");
}

void Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{}
