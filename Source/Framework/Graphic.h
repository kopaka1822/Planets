#pragma once
#include "../SDL/SDL.h"
#include "Drawing.h"

class Graphic
{
public:
	Graphic();
	~Graphic();
	void Create(SDL_GLContext context, int width, int height);
	void Dispose();

	void BeginFrame();
	void EndFrame();
	void Resize(int width, int height);

	Graphic& GetGraphic();

	Drawing draw;
private:

	void ApplyResize();
private:
	SDL_GLContext glContext;
	glm::mat4 curProj;
	float curWidth, curHeight;

	PointI newSize;
	bool bResizeQueued = false;

	bool bInitialized = false;

	// resizing
	float fovy;
	float aspect;
	float scalar;
	float newWidth;
	float newHeight;
};
