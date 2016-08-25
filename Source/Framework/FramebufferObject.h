#pragma once
#include "../glew/glew.h"
#include <exception>
#include "Shader.h"

// OpenGL Version 3.0
class FramebufferObject
{
public:
	~FramebufferObject()
	{
		Dispose();
	}
	void SetTextureFilter(GLint filter)
	{
		assert(colorTex);

		glBindTexture(GL_TEXTURE_2D, colorTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

		GLCheck("framebuffer set filter");
	}
	void CreateAndApply(int width, int heigth, GLint filter = GL_NEAREST)
	{
		curWidth = width;
		curHeight = heigth;

		assert(colorTex == -1);

		glGenTextures(1, &colorTex);
		glBindTexture(GL_TEXTURE_2D, colorTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, (GLsizei)curWidth, (GLsizei)curHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		glGenFramebuffers(1, &fb);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
		// attach 2d texture to framebuffer
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, colorTex, 0);

		GLenum status;
		status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		switch (status)
		{
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			break;
		case GL_FRAMEBUFFER_UNDEFINED:
			throw Exception("GL_FRAMEBUFFER_UNDEFINED");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
			throw Exception("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
			throw Exception("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT");
			break;

		default:
			throw Exception("no framebuffer support?");
		}
		//-------------------------
		//and now you can render to GL_TEXTURE_2D
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
	}
	void SaveTexture()
	{
		// end drawing
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

		// now we can use the texture
	}
	// this will bind texture slot 0
	void DrawTextureInShader(Shader& sh, GLenum sFactor = GL_SRC_ALPHA, GLenum dFactor = GL_ONE_MINUS_SRC_ALPHA)
	{
		// standart drawing

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		sh.Bind();

		// draw Texture
		glEnable(GL_BLEND);
		glBlendFunc(sFactor, dFactor);
		glActiveTexture(GL_TEXTURE0);

		// Bind texture
		glBindTexture(GL_TEXTURE_2D, colorTex);

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		glBegin(GL_TRIANGLE_STRIP);
		{
			glVertex2f(-1.0, -1.0f);
			glVertex2f(1.0f, -1.0f);
			glVertex2f(-1.0f, 1.0f);
			glVertex2f(1.0f, 1.0f);
		}
		GLEndSafe();

		glDisable(GL_BLEND);

		sh.Undbind();
	}
	void BindTexture(int slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);

		// Bind texture
		glBindTexture(GL_TEXTURE_2D, colorTex);
	}
	GLuint GetRawTexture()
	{
		return colorTex;
	}
	void Dispose()
	{
		// cleanup
		if (signed(colorTex) != -1)
		{
			glDeleteTextures(1, &colorTex);
			colorTex = -1;
		}

		if (signed(fb) != -1)
		{
			//Bind 0, which means render to back buffer, as a result, fb is unbound
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
			glDeleteFramebuffersEXT(1, &fb);
			fb = -1;
		}
	}
private:
	GLuint colorTex = -1;
	GLuint fb = -1;
	int curWidth, curHeight;
};
