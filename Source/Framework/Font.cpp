#include "Font.h"
#include "../Utility/FileException.h"

void Font::Load(FT_Library ft, const std::string& name, float scalar)
{
	this->ft = ft;
	long fileSize = 0;

	FILE* pFile = fopen(name.c_str(), "rb");
	{
		if (!pFile)
			throw ExMissingFile(name);

		// obtain file size:
		fseek(pFile, 0, SEEK_END);
		fileSize = ftell(pFile);
		rewind(pFile);

		fontData = std::unique_ptr<char[]>(new char[fileSize]);

		auto count = fread(fontData.get(), 1, fileSize, pFile);
		if (count != fileSize)
			throw ExInvalidFile(name, "could not complete font file reading");

		fclose(pFile);
		pFile = nullptr;
	}

	// create new memory face
	FT_Error fterr = FT_New_Memory_Face(ft, (FT_Byte*)fontData.get(), fileSize,
		0, &face);
	if (fterr)
		// TOOD throw invalid file exception
		throw ExInvalidFile(name, "invalid font file");

	fterr = FT_Set_Pixel_Sizes(face, 0, int(scalar * 1.5f)); // size in pixel (quality of bitmap)
	if (fterr)
		throw Exception("could not set font pixel size");

	FindMaxBearing(scalar);


	// Shader
	Shader::Load("data/shader/text"); // Load shader data
}
void Font::Create()
{
	Shader::Create();

	LocateUniforms();

	LoadTextures();

	GLCheck("Font texture");
}
void Font::Dispose()
{
	glDeleteTextures(CHAR_END - CHAR_START, texarray);
	memset(texarray, 0, sizeof(texarray));

	Shader::Dispose();
}
