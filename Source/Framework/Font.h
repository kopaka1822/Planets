#pragma once
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <ft2build.h>
//#include "../freetype/ft2build.h"
#include FT_FREETYPE_H
//#include "../freetype/freetype/freetype.h"
#include <string>
#include "Shader.h"
#include "../Utility/Point.h"
#include "Shader.h"
#include "../glm/gtx/transform.hpp"
#include <algorithm>
#include <memory>
#include "Color.h"

class Font : public Shader
{
public:
	Font()
	{}
	void Load(FT_Library ft, const std::string& name, float scalar);
	void Dispose();
	void Create();
	~Font()
	{}
	void Text(const std::string& txt, PointF pos)
	{
		GLCheck("before font");
		Shader::Bind();

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glActiveTexture(GL_TEXTURE0);

		// Loop through all chars
		for (const auto& c : txt)
		{
			if (c < CHAR_START || c > CHAR_END)
				continue;

			const FT_Glyph_Metrics& m = metrics[c];


			float bmpHeight = m.height * scaleFactor;
			float offHeight = (maxBearing - m.horiBearingY) * scaleFactor;

			float bmpWidth = m.width * scaleFactor;
			float advance = m.horiAdvance * scaleFactor;
			float bearX = m.horiBearingX * scaleFactor;


			glBindTexture(GL_TEXTURE_2D, texarray[c - CHAR_START]);

			GLCheck("before glBegin");
			glBegin(GL_TRIANGLE_STRIP);
			{
				glVertex4f(pos.x + bearX, pos.y + offHeight, 0, 0);
				glVertex4f(pos.x + bearX + bmpWidth, pos.y + offHeight, 1, 0);
				glVertex4f(pos.x + bearX, pos.y + offHeight + bmpHeight, 0, 1);
				glVertex4f(pos.x + bearX + bmpWidth, pos.y + offHeight + bmpHeight, 1, 1);
			}
			GLEndSafe();

			/* Advance the cursor to the start of the next character */
			pos.x += advance;
		}

		glDisable(GL_BLEND);

		//glBindTexture(GL_TEXTURE_2D, 0);

		Shader::Undbind();
		GLCheck("after font");
	}
	PointF GetMetrics(const char* txt) const
	{
		float width = 0.0f;
		while (*txt != '\0')
		{
			width += metrics[*txt].horiAdvance;
			txt++;
		}
		return PointF(width * scaleFactor, fontH);
	}
	PointF GetMetrics(const std::string& txt) const
	{
		return GetMetrics(txt.c_str());
	}
	void Text(const char* txt, const PointF& pos)
	{
		Text(std::string(txt), pos);
	}
	float GetFontHeight() const
	{
		return fontH;
	}
	float GetFontWidth() const
	{
		return fontWi;
	}
	void SetColor(Color c)
	{
		glUseProgram(program);
		glUniform4f(colLocation, c.r, c.g, c.b, c.a);
		glUseProgram(0);
	}
private:
	void FindMaxBearing(float scale)
	{
		fontH = scale;
		// iterate through ascii chars
		memset(&metrics, 0, sizeof(metrics));

		maxBearing = 0;
		int maxDown = 0; // e.g. the bottom part of g
		int maxAdv = 0;
		for (int i = CHAR_START; i <= CHAR_END; i++)
		{
			if (FT_Load_Char(face, i, FT_LOAD_RENDER))
				continue;

			// Get Metrics
			metrics[i] = face->glyph->metrics;
			metrics[i].horiAdvance = face->glyph->advance.x;
			maxBearing = std::max(maxBearing, (int)metrics[i].horiBearingY);
			maxDown = std::max(maxDown, int(metrics[i].height - metrics[i].horiBearingY));
			maxAdv = std::max(maxAdv, int(metrics[i].horiAdvance));
		}

		maxHeight = maxBearing + maxDown;
		scaleFactor = scale / maxHeight;
		fontWi = maxAdv * scaleFactor;
	}
	void LocateUniforms()
	{
		texLocation = glGetUniformLocation(program, "tex");
		if (texLocation == -1)
			throw Exception("font shader: tex uniform not found");

		colLocation = glGetUniformLocation(program, "color");
		if (colLocation == -1)
			throw Exception("font shader: tex uniform not found");

		glUseProgram(program);
		glUniform1i(texLocation, 0);
		glUniform4f(colLocation, 1.0f, 1.0f, 1.0f, 1.0f);
		glUseProgram(0);

		GLCheck("font uniforms");
	}
	void LoadTextures()
	{
		memset(texarray, 0, sizeof(texarray));
		glGenTextures(CHAR_END - CHAR_START, texarray);

		FT_GlyphSlot g = face->glyph;

		for (int i = CHAR_START; i <= CHAR_END; i++)
		{
			int index = i - CHAR_START;

			glBindTexture(GL_TEXTURE_2D, texarray[index]);

			/* We require 1 byte alignment when uploading texture data */
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			/* Clamping to edges is important to prevent artifacts when scaling */
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			/* Linear filtering usually looks best for text */
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


			if (FT_Load_Char(face, i, FT_LOAD_RENDER))
			{
				texarray[index] = 0;
				continue;
			}

			/* Upload the "bitmap", which contains an 8-bit grayscale image, as an alpha texture */
			glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, g->bitmap.width, g->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);
		}
		
	}
private:
	static const int CHAR_START = 32;
	static const int CHAR_END = 126;

	FT_Library ft = nullptr;
	FT_Face face = nullptr;
	GLint texLocation;
	GLint colLocation;

	std::unique_ptr<char[]> fontData;
	GLuint texarray[CHAR_END - CHAR_START];

	static const int nChars = 256;
	FT_Glyph_Metrics metrics[nChars];

	float scaleFactor = 0.0f;
	int maxBearing = 0;
	int maxHeight = 0;

	float fontH; // max Font height
	float fontWi; // maxFont width
};
