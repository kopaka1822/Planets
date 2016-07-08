#pragma once
#include "Shader.h"
#include "../Utility/Rect.h"
#include <memory>
#include "ShaderDisk.h"
#include "Texture.h"
#include "ShaderCirlce.h"
#include "ShaderCool.h"
#include "FramebufferObject.h"
#include "ShaderBrightFilter.h"
#include "ShaderTexAdd.h"
#include "ShaderRadialBlur.h"
#include "Font.h"
#include "Database.h"
#include "ShaderStory.h"
#include "ShaderCartoon.h"
#include "ShaderPlanet.h"
#include "../Utility/Timer.h"
#include "ShaderWave.h"
#include "ShaderFXAA.h"
#include "ShaderGlow.h"

class Drawing
{
	friend class Graphic;

	struct ColVertex
	{
		PointF pos;
		Color col;
	};
public:
	enum ShaderMode
	{
		RadialBlur = 1,
		Storybook = 2,
		FXAA = 4
	};

	enum E_FONT
	{
		F_BIG,
		F_MEDIUM,
		F_SMALL
	};

	struct ParticleVertex
	{
		PointF pos;
		Color col;
	};
public:
	Drawing();
	void LoadStartupData();
	~Drawing();

	void Create();
	void Dispose();
	
	void SetPerspective(const glm::mat4& m);

	// Drawing functions
	void DrawRect(const RectF& r, Color color);
	void DrawBox(const RectF& r, float border, Color colBor, Color colIns);
	void DrawTransBox(const RectF& r, float border, Color c);

	void DrawLine(PointF p1, PointF p2, Color color, float strokeWidth);

	void DrawHLine(PointF start, float width, float strokeWidth, Color c);
	void DrawVLine(PointF start, float height, float strokeWidth, Color c);

	void DrawCircle(const PointF& center, float radius, Color c, float stroke);
	void DrawDisc(const PointF& center, float radius, Color c);
	
	void DrawSprite(const RectF& r,const Texture& tex);
	void DrawSpriteColored(const RectF& r, const Texture& tex, Color c);

	void DrawDiscHSV(const PointF& center, float radius);
	void DrawTriangleHSV(const PointF& center, float radius, Color c);

	Font& GetFont(E_FONT font);
	static Drawing& GetDraw();

	void DrawParticle(PointF pos, Color color, float r, Database::GameTex tex = Database::GameTex::Particle);
	void DrawParticleArray(const PointF* pArray, unsigned int len, Color c, float size, Database::GameTex tex = Database::GameTex::Particle,
		PointF translation = PointF(0.0f,0.0f), float scale = 1.0f, PointF ul = PointF(0.0f,0.0f));
	
	//void DrawParticleArray(const PointF* pArray, const Color* pColors, unsigned int len, float size, Database::GameTex tex = Database::GameTex::Particle,
	//	PointF translation = PointF(0.0f, 0.0f), float scale = 1.0f, PointF ul = PointF(0.0f, 0.0f));

	void DrawPlanet(PointF center, Color c, float r);

	// last Spawn  in seconds
	void DrawPlanet(PointF center, Color cmain, Color csub, float r, float lastSpawn, int id = 0);

	void SetShaderConfig(unsigned int shCon)
	{
		newShaderConfig = shCon;
	}
	unsigned int GetShaderConfig() const
	{
		return newShaderConfig;
	}

	void ApplyPostProcessing();
	void PrepareImageFilter();
	void DrawBackground();

	Texture* MakeTexture(const std::string& filename);

	void SetMetrics(float width, float height, int rWi, int rHi)
	{
		curWidth = width;
		curHeight = height;
		realWidth = rWi;
		realHeight = rHi;
	}
	void SetBlur(PointF center);

	static Color PickHSV(float angle);

	bool HasFramebufferSupport() const
	{
		return oglMajorVersion >= 3;
	}
private:
	void LoadShaders(); // this has to be done once

	void DrawGlow(const PointF& center, float radius, const Color& col);
	void DrawBoxShadered(const PointF& center, float radius, Shader& shader, GLenum blendMode = GL_ONE_MINUS_SRC_ALPHA);
	void InitParticels(const Color& col, float size, Texture& tex);
	void ShutParticles();
	void InitHSVTriangle();

	//void ApplyGlowing(FramebufferObject* pFbo);
	void ApplyStorybook(FramebufferObject& srcFbo);
	void ApplyRadial(FramebufferObject& srcFbo);
	void ApplyFXAA(FramebufferObject& srcFbo);
	Database::GameTex ChooseTexture(Database::GameTex in);
private:
	Texture texStory;
	Texture texNoise;

	ShaderDisk shDisk;
	ShaderCircle shCircle;
	ShaderRadialBlur shRadBlur;
	ShaderStory shStory;
	ShaderPlanet shPlanet;
	ShaderFXAA shFXAA;
	ShaderTexAdd shTexAdd;
	ShaderGlow shGlow;

	float curWidth, curHeight; // transformed w and h
	int realWidth, realHeight; // actual width and height of window

	FramebufferObject fbo;

	// radial blur
	PointF blurCenter;
	float curBlur;
	bool isBlurring = false;

	struct FT_LibraryRec_  * ft;

	Font fontBig;
	Font fontMed;
	Font fontSmall;

	std::vector< std::unique_ptr< Texture >> textures;
	std::vector< ColVertex > hsvCoords;

	unsigned int curShaderConfig = 0;
	unsigned int newShaderConfig = 0;

	PointF planmove;

	Timer dTime;

	int oglMajorVersion = -1;
	int oglMinorVersion = -1;
	float oglMaxPointsize = 0.0f;
};

#define FONT_SMALL  (Drawing::GetDraw().GetFont(Drawing::F_SMALL))
#define FONT_MED  (Drawing::GetDraw().GetFont(Drawing::F_MEDIUM))
#define FONT_BIG  (Drawing::GetDraw().GetFont(Drawing::F_BIG))
