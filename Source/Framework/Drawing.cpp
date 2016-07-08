#include "Drawing.h"
#include "../System/Log.h"
#include "../glm/gtx/transform.hpp"
#include "../System/System.h"
#include "Framework.h"
#include <ft2build.h>
#include FT_FREETYPE_H

Drawing* curDraw = nullptr; // pointer to current drawing object

Drawing& Drawing::GetDraw()
{
	assert(curDraw);
	return *curDraw;
}
Drawing::Drawing()
{
	newShaderConfig = Settings::GetShader();
	planmove = { 0.0f, 0.2f };
}

Drawing::~Drawing()
{}

void Drawing::LoadStartupData()
{
	glGetIntegerv(GL_MAJOR_VERSION, &oglMajorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &oglMinorVersion);
	auto err = glGetError();
	if (err == GL_INVALID_ENUM)
	{
		// assume 2.1
		oglMajorVersion = 2;
		oglMinorVersion = 1;
	}


	glGetFloatv(GL_POINT_SIZE_MAX_ARB, &oglMaxPointsize);
	GLCheck("GL_POINT_SIZE_MAX_ARB check");
	// has to be executed once!
	LoadShaders();

	// textures
	//texPlanet.Load("data/pic/planet.tga");
	texStory.Load("data/pic/story.tga");
	texNoise.Load("data/pic/perlin.bmp");

	// Load Fonts
	if (FT_Init_FreeType(&ft))
		throw Exception("FreeType init failed");

	// Blanka-Regular.otf
	// FreeSans.ttf
	// Consolas.ttf

	fontBig.Load(ft, "data/font/Blanka-Regular.otf", 80.0f);
	fontMed.Load(ft, "data/font/Consolas.ttf", 50.0f);
	fontSmall.Load(ft, "data/font/Consolas.ttf", 30.0f);

	GLCheck("loaded fonts");

	InitHSVTriangle();

	dTime.StartWatch();

	curDraw = this;
}

Texture* Drawing::MakeTexture(const std::string& filename)
{
	std::unique_ptr< Texture > pTex;
	pTex = std::unique_ptr< Texture >(new Texture());
	pTex->Load(filename);

	pTex->Create();
	textures.push_back(std::move(pTex));
	return textures.back().get();
}

void Drawing::Create()
{
	Log::Write("creating openGL shaders");
	{
		shDisk.Create();
		shCircle.Create();
		shRadBlur.Create();
		shStory.Create();
		shPlanet.Create();
		shFXAA.Create();
		shTexAdd.Create();
		shGlow.Create();
	}

	Log::Write("creating openGL textures");
	{
		//texPlanet.Create();
		texStory.Create();
		texNoise.Create();

		for (auto& t : textures)
			t->Create();
	}

	Log::Write("creating freetype font textures");
	{
		fontBig.Create();
		fontMed.Create();
		fontSmall.Create();
	}

	GLCheck("create");
}

void Drawing::Dispose()
{
	Log::Write("deleting openGL textures");
	{
		//texPlanet.Dispose();
		texStory.Dispose();
		texNoise.Dispose();

		for (auto& t : textures)
			t->Dispose();
	}

	Log::Write("deleting freetype font textures");
	{
		fontBig.Dispose();
		fontMed.Dispose();
		fontSmall.Dispose();
	}

	Log::Write("deleting openGL shaders");
	{
		shDisk.Dispose();
		shCircle.Dispose();
		shRadBlur.Dispose();
		shStory.Dispose();
		shPlanet.Dispose();
		shFXAA.Dispose();
		shTexAdd.Dispose();
		shGlow.Dispose();
	}

	GLCheck("Dispose");
}

void Drawing::PrepareImageFilter()
{
	float dt = dTime.GetTimeSecond();
	dTime.StartWatch();

	//planmove.x += 0.05f * dt;
	planmove.x += 0.8f * dt;
	planmove.y += 0.005f * dt;

	if (planmove.x > 1.0f)
		planmove.x -= 1.0f;
	if (planmove.y > 1.0f)
		planmove.y -= 1.0f;

	curShaderConfig = newShaderConfig;

	if (oglMajorVersion >= 3) // support since version 3.0
	{
		if (curShaderConfig)
		{
			// some shader is enabled

			fbo.CreateAndApply(realWidth, realHeight);
		}
	}
}

void Drawing::DrawBackground()
{

}

void Drawing::ApplyPostProcessing()
{
	if (!curShaderConfig || oglMajorVersion < 3)
		return; // no shaders

	// cur Frame in fbo
	fbo.SaveTexture();

	FramebufferObject* curFbo = &fbo;
	FramebufferObject fboTmp1;
	FramebufferObject fboTmp2;
	
	int shaderCount = 0;

	if (curShaderConfig & RadialBlur)
	{
		if (curShaderConfig & ~RadialBlur)
		{
			// other shaders to apply
			fboTmp1.CreateAndApply(realWidth, realHeight);
			{
				ApplyRadial(fbo);
			}
			fboTmp1.SaveTexture();

			curFbo = &fboTmp1;
			shaderCount++;
		}
		else
		{
			// only shader
			ApplyRadial(fbo);
		}
	}

	if (curShaderConfig & Storybook)
	{
		if (curShaderConfig & FXAA)
		{
			FramebufferObject* pDst = &fboTmp1;
			if (shaderCount == 1)
				pDst = &fboTmp2;

			pDst->CreateAndApply(realWidth, realHeight);
			{
				ApplyStorybook(*curFbo);
			}
			pDst->SaveTexture();

			curFbo = pDst;
			shaderCount++;
		}
		else
		{
			// last Shader
			ApplyStorybook(*curFbo);
		}
	}

	if (curShaderConfig & FXAA)
	{
		// last shader
		ApplyFXAA(*curFbo);
	}

	fbo.Dispose();
	fboTmp1.Dispose();
	fboTmp2.Dispose();
}
void Drawing::ApplyFXAA(FramebufferObject& srcFbo)
{
	shFXAA.SetStep(PointF(1.0f / realWidth, 1.0f / realHeight));

	srcFbo.SetTextureFilter(GL_LINEAR);
	srcFbo.DrawTextureInShader(shFXAA);
}

void Drawing::ApplyRadial(FramebufferObject& srcFbo)
{
	// already saved in texture
	if (isBlurring)
	{
		curBlur += 0.025f;
		float sinblur = sinf(curBlur);
		sinblur = std::max(sinblur, 0.0f);
		float sinsq = sinblur * sinblur * sinblur;

		shRadBlur.SetParameters(blurCenter, PointF(1.0f / realWidth, 1.0f / realHeight),
			-sinsq, 1.0f + sinblur * 3.0f);
		srcFbo.DrawTextureInShader(shRadBlur);

		if (sinblur == 0.0f)
			isBlurring = false;
	}
	else
	{
		// draw normal
		shTexAdd.SetFactor(1.0f);
		srcFbo.DrawTextureInShader(shTexAdd);
	}
	GLCheck("apply image radial");
}


void Drawing::ApplyStorybook(FramebufferObject& srcFbo)
{
	PointF step = PointF(1.0f / realWidth, 1.0f / realHeight);
	shStory.SetStep(step);


	texStory.Bind(1);
	srcFbo.DrawTextureInShader(shStory);
	
	GLCheck("apply image story");
}

/*
void Drawing::ApplyGlowing(FramebufferObject* pFbo)
{
	fbo.SaveTexture(); // this is the original scene
	// drawing texture


	
		Steps:
			1.	DrawScene
			2.	Filter bright elements
			3.	blur horizontal
			4.	blur vertical
			5.	add images together
	

	// this will store the result of the brightness filter
	// 2.	Filter bright elements
	


	//if (pFbo)
		//pFbo->CreateAndApply(realWidth, realHeight);
	
	
	FramebufferObject fbo2;
	fbo2.CreateAndApply(realWidth, realHeight);
	{
		shFXAA.SetStep(PointF(1.0f / realWidth, 1.0f / realHeight));
		fbo.DrawTextureInShader(shFXAA);
	//}
	//fbo2.SaveTexture();
	//
	//fbo2.DrawTextureInShader(shFXAA);


	//if (pFbo)
		//pFbo->SaveTexture();

	//FramebufferObject onlyBright;
	//onlyBright.CreateAndApply(realWidth, realHeight);
	//{
	//	fbo.DrawTextureInShader(shBrightFilter);
	//}
	//onlyBright.SaveTexture();

	//// 3.	blur horizontal
	//FramebufferObject blur1;
	//blur1.CreateAndApply(realWidth, realHeight);
	//{
	//	shBlur.SetParameters(PointF(1.0f / realWidth, 0.0f));
	//	onlyBright.DrawTextureInShader(shBlur);
	//}
	//blur1.SaveTexture();

	//// 4.	blur vertical
	//FramebufferObject blur2;
	//blur2.CreateAndApply(realWidth, realHeight);
	//{
	//	shBlur.SetParameters(PointF(0.0f, 1.0f / realHeight));
	//	blur1.DrawTextureInShader(shBlur);
	//}
	//blur2.SaveTexture();

	//// 5.	add images together
	//// now combine original with blurred
	///*if (isBlurring)
	//{
	//	FramebufferObject radial;
	//	radial.CreateAndApply(realWidth, realHeight, GL_LINEAR);
	//	{
	//		shTexAdd.SetFactor(1.0f);
	//		fbo.DrawTextureInShader(shTexAdd);
	//		shTexAdd.SetFactor(1.0f);
	//		blur2.DrawTextureInShader(shTexAdd, GL_SRC_ALPHA, GL_ONE);
	//	}
	//	radial.SaveTexture();

	//	curBlur += 0.025f;
	//	float sinblur = sinf(curBlur);
	//	sinblur = std::max(sinblur, 0.0f);
	//	float sinsq = sinblur * sinblur * sinblur;

	//	shRadBlur.SetParameters(blurCenter, PointF(1.0f / realWidth, 1.0f / realHeight),
	//		-sinsq, 1.0f + sinblur * 3.0f);
	//	radial.DrawTextureInShader(shRadBlur);

	//	if (sinblur == 0.0f)
	//		isBlurring = false;
	//}
	//else
	//{
	//	if (pFbo)
	//		pFbo->CreateAndApply(realWidth, realHeight);
	//	{
	//		shTexAdd.SetFactor(1.0f);
	//		fbo.DrawTextureInShader(shTexAdd);
	//		shTexAdd.SetFactor(1.0f);
	//		blur2.DrawTextureInShader(shTexAdd, GL_SRC_ALPHA, GL_ONE);
	//	}
	//	if (pFbo)
	//		pFbo->SaveTexture();
	//}

	//onlyBright.Dispose();
	//blur1.Dispose();
	//blur2.Dispose();
	fbo.Dispose();
	GLCheck("apply image");
}*/
void Drawing::LoadShaders()
{
	Log::Write("loading shaders");

	shDisk.Load();
	shCircle.Load();
	shRadBlur.Load();
	shStory.Load();
	shPlanet.Load();
	shFXAA.Load();
	shTexAdd.Load();
	shGlow.Load();

	Log::Write("shaders loaded");
}
void Drawing::SetPerspective(const glm::mat4& m)
{

}
Font& Drawing::GetFont(E_FONT font)
{
	if (font == F_BIG)
	{
		return fontBig;
	}
	else if (font == F_MEDIUM)
	{
		return fontMed;
	}
	else
	{
		return fontSmall;
	}
}
void Drawing::DrawBox(const RectF& r, float border, Color colBor, Color colIns)
{
	DrawRect(r, colBor);
	DrawRect(RectF(r.x1 + border, r.y1 + border, r.x2 - border, r.y2 - border), colIns);
}
void Drawing::DrawRect(const RectF& r, Color color)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor4f(color.r, color.g, color.b, color.a);
	glBegin(GL_TRIANGLE_STRIP);
	{
		glVertex2f(r.x1, r.y1);
		glVertex2f(r.x2, r.y1);
		glVertex2f(r.x1, r.y2);
		glVertex2f(r.x2, r.y2);
	}
	GLEndSafe();
}
void Drawing::DrawSprite(const RectF& r,const Texture& tex)
{
	DrawSpriteColored(r, tex, Color::White());
}
void Drawing::DrawSpriteColored(const RectF& r, const Texture& tex, Color c)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	tex.Bind(0);

	glColor4f(c.r, c.g, c.b, c.a);
	glBegin(GL_TRIANGLE_STRIP);
	{
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(r.x1, r.y1);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(r.x2, r.y1);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(r.x1, r.y2);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(r.x2, r.y2);
	}
	GLEndSafe();


	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void Drawing::DrawHLine(PointF start, float width, float strokeWidth, Color c)
{
	PointF off(strokeWidth / 2.0f, strokeWidth / 2.0f);
	DrawRect(RectF(start - off, start + off + PointF(width, 0.0f)), c);
}
void Drawing::DrawVLine(PointF start, float height, float strokeWidth, Color c)
{
	PointF off(strokeWidth / 2.0f, strokeWidth / 2.0f);
	DrawRect(RectF(start - off, start + off + PointF(0.0f, height)), c);
}
void Drawing::DrawTransBox(const RectF& r, float border, Color c)
{
	DrawRect(RectF(r.TopLeft(), r.TopRight() + PointF(0.0f, border)), c);
	DrawRect(RectF(r.BottomLeft() + PointF(0.0f, -border), r.BottomRight()), c);
	DrawRect(RectF(r.TopLeft(), r.BottomLeft() + PointF(border, 0.0f)), c);
	DrawRect(RectF(r.TopRight() + PointF(-border, 0.0f), r.BottomRight()), c);
}
void Drawing::DrawLine(PointF p1, PointF p2, Color color, float strokeWidth)
{
	if (p2.x < p1.x)
		std::swap(p1, p2);

	glm::mat4 trans = glm::translate(glm::mat4(), glm::vec3(p1.x, p1.y, 0.0f));
	trans = glm::rotate(trans, atanf((p2.y - p1.y) / (p2.x - p1.x)), glm::vec3(0.0f, 0.0f, 1.0f)); // dy / dx
	trans = glm::translate(trans, glm::vec3(-strokeWidth / 2.0f, -strokeWidth / 2.0f, 0.0f));
	trans = glm::scale(trans, glm::vec3((p1 - p2).length() + strokeWidth, strokeWidth, 1.0f));

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(&trans[0][0]);

	glColor4f(color.r, color.g, color.b, color.a);
	glBegin(GL_TRIANGLE_STRIP);
	{
		glVertex2f(0.0f, 1.0f);
		glVertex2f(1.0f, 1.0f);
		glVertex2f(0.0f, 0.0f);
		glVertex2f(1.0f, 0.0f);
	}
	GLEndSafe();
}

void Drawing::DrawDisc(const PointF& center, float radius, Color c)
{
	shDisk.SetParameters(c);

	DrawBoxShadered(center, radius, shDisk);
}
void Drawing::DrawCircle(const PointF& center, float radius, Color c, float stroke)
{
	float inrad = 0.5f - (stroke / (radius * 2.0f));
	shCircle.SetParameters(c, inrad);

	DrawBoxShadered(center, radius, shCircle);
}
void Drawing::DrawBoxShadered(const PointF& center, float radius, Shader& shader, GLenum blendMode)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(center.x, center.y, 0.0f);
	glScalef(radius * 2.0f, radius * 2.0f, 1.0f);
	glTranslatef(-0.5f, -0.5f, 0.0f);

	shader.Bind();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, blendMode);

	glBegin(GL_TRIANGLE_STRIP);
	{
		glVertex2f(0.0f, 1.0f);
		glVertex2f(1.0f, 1.0f);
		glVertex2f(0.0f, 0.0f);
		glVertex2f(1.0f, 0.0f);
	}
	GLEndSafe();

	glDisable(GL_BLEND);
	shader.Undbind();
}
Database::GameTex Drawing::ChooseTexture(Database::GameTex in)
{
	if (curShaderConfig & Storybook)
	{
		// transfer textres
		switch (in)
		{
		case Database::Particle:
			return Database::StoryPart;
		case Database::ParticleExpl:
			return Database::StoryBomb;
		case Database::ParticleTank:
			return Database::StorySpeed;
		case Database::ParticleSabo:
			return Database::StorySabo;
		}
	}
	return in;
}
void Drawing::DrawParticle(PointF pos, Color color, float r, Database::GameTex tex)
{
	Texture& texture = Database::GetTexture(ChooseTexture(tex));
	InitParticels(color, r, texture);
	{
		GLCheck("before glBegin");
		glBegin(GL_POINTS);
		{
			glVertex2f(pos.x, pos.y);
		}
		GLEndSafe();
	}
	ShutParticles();
}
void Drawing::DrawParticleArray(const PointF* pArray, unsigned int len, Color color, float size, Database::GameTex tex, PointF translation, float scale, PointF ul)
{
	if (len == 0)
		return;

	Texture& texture = Database::GetTexture(ChooseTexture(tex));
	InitParticels(color, size * scale, texture);

	// do translation
	glTranslatef(ul.x, ul.y, 0.0f);
	glScalef(scale, scale, 1.0f);
	glTranslatef(-translation.x, -translation.y, 0.0f);
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		{
			glVertexPointer(2, GL_FLOAT, 0, pArray);

			glDrawArrays(GL_POINTS, 0, len);
		}
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	ShutParticles();
}
//void Drawing::DrawParticleArray(const PointF* pArray, const Color* pColors, unsigned int len, float size, Database::GameTex tex, PointF translation, float scale, PointF ul)
//{
//	if (len == 0)
//		return;
//
//	Texture& texture = Database::GetTexture(ChooseTexture(tex));
//	InitParticels(Color::White(), size * scale, texture); // pass dummy color
//
//	// do translation
//	glTranslatef(ul.x, ul.y, 0.0f);
//	glScalef(scale, scale, 1.0f);
//	glTranslatef(-translation.x, -translation.y, 0.0f);
//	{
//		glEnableClientState(GL_COLOR_ARRAY);
//		glEnableClientState(GL_VERTEX_ARRAY);
//		{
//			glColorPointer(4, GL_FLOAT, 0, pColors);
//			glVertexPointer(2, GL_FLOAT, 0, pArray);
//
//
//			glDrawArrays(GL_POINTS, 0, len);
//		}
//		glDisableClientState(GL_VERTEX_ARRAY);
//		glDisableClientState(GL_COLOR_ARRAY);
//	}
//	ShutParticles();
//}
void Drawing::InitParticels(const Color& col, float size, Texture& tex)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
	tex.Bind(0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glEnable(GL_POINT_SPRITE_ARB);
	glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);

	//glEnable(GL_PROGRAM_POINT_SIZE);
	size = size * Framework::GetScalar();
	//size = std::min(size, oglMaxPointsize);

	glPointSize(size);
	glPointParameterfARB(GL_POINT_SIZE_MAX_ARB, size);
	glPointParameterfARB(GL_POINT_SIZE_MIN_ARB, 1.0f);

	glColor4f(col.r, col.g, col.b, col.a);
}
void Drawing::ShutParticles()
{
	glDisable(GL_POINT_SPRITE_ARB);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	GLCheck("Particles");
}
void Drawing::DrawPlanet(PointF center, Color c, float r)
{
	//DrawSpriteColored(RectF::FromPoint(center, r), texPlanet, c);

	// draw box shadered

	

	DrawPlanet(center, c, c, r, 10000.0f);
}
void Drawing::DrawPlanet(PointF center, Color cmain, Color csub, float r, float lastSpawn, int id)
{
	/*DrawPlanet(center, cmain, r);

	DrawSpriteColored(RectF::FromPoint(center, r), Database::GetTexture(Database::GameTex::PlanetDes), csub);*/
	lastSpawn = tool::clamp(lastSpawn, 0.0f, 1.0f);

	if (Settings::PlanetGlowEnabled())
		DrawGlow(center, r * 1.9f, csub);

	// draw spawning shockwave?

	

	texNoise.Bind(0);
	shPlanet.SetParameters(cmain, csub, PointF(1.0f - lastSpawn,planmove.y));

	DrawBoxShadered(center, r * 0.92f, shPlanet);// draw a little bit smaller so units dont stuck in planet

	texNoise.Unbind();
}
void Drawing::DrawGlow(const PointF& center, float radius, const Color& col)
{
	if (curShaderConfig & Storybook)
		return; // that would be ugly

	texNoise.Bind(0);
	shGlow.SetParameters(col, planmove);

	DrawBoxShadered(center, radius, shGlow, GL_ONE);

	texNoise.Unbind();
}
void Drawing::SetBlur(PointF center)
{
	// adjust from framework to (0,1)
	PointF d1 = (center - Framework::DrawStart());
	PointF d2 = (Framework::DrawWidth() - Framework::DrawStart());

	blurCenter = PointF(d1.x / d2.x, 1.0f - d1.y / d2.y);

	curBlur = float(M_PI) / 2.0f;

	if (curShaderConfig & RadialBlur)
		isBlurring = true;
	else
		isBlurring = false;
}

void Drawing::DrawDiscHSV(const PointF& center, float radius)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(center.x, center.y, 0.0f);
	glScalef(radius, radius, 1.0f);
	
	glBegin(GL_TRIANGLE_FAN);
	{
		for (const auto& e : hsvCoords)
		{
			glColor3f(e.col.r, e.col.g, e.col.b);
			glVertex2f(e.pos.x, e.pos.y);
		}
	}
	GLEndSafe();
}
void Drawing::DrawTriangleHSV(const PointF& center, float radius, Color c)
{
	DrawDisc(center, radius, Color::Black());

	// draw triangele

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(center.x, center.y, 0.0f);
	glScalef(radius, radius, 1.0f);

	glBegin(GL_TRIANGLES);
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex2f(-0.707f, 0.707f);
		glColor3f(c.r, c.g, c.b);
		glVertex2f(1.0f, 0.0f);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex2f(-0.707f, -0.707f);
	}
	GLEndSafe();
}
Color Drawing::PickHSV(float angle)
{
	static const float pi2 = 2.0f * float(M_PI);

	Color col;
	col.a = 1.0f;

	float h = angle / pi2;
	if (h >= 1.0f)
		h -= 1.0f;
	h = std::max(0.0f, h);

	col.r = tool::clamp((float)fabs(6.0f * h - 3.0f) - 1.0f, 0.0f, 1.0f);
	col.g = tool::clamp((float)(2.0f - fabs(6.0f * h - 2.0f)), 0.0f, 1.0f);
	col.b = tool::clamp((float)(2.0f - fabs(6.0f * h - 4.0f)), 0.0f, 1.0f);

	return col;
}
void Drawing::InitHSVTriangle()
{
	hsvCoords.clear();
	static const int samples = 100;

	hsvCoords.assign(samples + 3,ColVertex());

	hsvCoords[0].col = Color::White();
	hsvCoords[0].pos = PointF(0.0f, 0.0f);

	float step = (2.0f * float(M_PI)) / float(samples);
	float curAngle = 0.0f;

	for (unsigned int i = 0; i <= samples + 1; ++i)
	{
		// change pVertex[i+1]
		hsvCoords[i + 1].col = PickHSV(curAngle);
		hsvCoords[i + 1].pos = PointF(1.0f, 0.0f).rot(curAngle);
		curAngle += step;
	}
}
