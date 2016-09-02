#include "Graphic.h"
#include "../glew/glew.h"
#include "../Utility/GLEW_Exception.h"
#include "../System/Log.h"
#include <algorithm>
#include "../glm/gtx/transform.hpp"
#include "../Utility/GL_Exception.h"
#include "Framework.h"

static Graphic* pGraphic = nullptr;

Graphic& GetGraphic()
{
	return *pGraphic;
}
Graphic::Graphic()
{
	Log::Write("initializing glew");

	//glewExperimental = GL_TRUE;
	auto status = glewInit();
	if (status != GLEW_OK)
		throw GLEW_Exception("glew init", status);

	Log::Info("------------------------------ Graphic Info ------------------------------");
	Log::Info(std::string("Glew Version:\t") + (const char*)glewGetString(GLEW_VERSION));

	// print open GL information
	Log::Info(std::string("Vendor:\t\t") + (const char*)glGetString(GL_VENDOR));
	Log::Info(std::string("Renderer:\t") + (const char*)glGetString(GL_RENDERER));
	Log::Info(std::string("OpenGL Version:\t") + (const char*)glGetString(GL_VERSION));
	Log::Info(std::string("Shading Lang.:\t") + (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
	
	// Query for the max point size supported by the hardware
	float maxSize = 0.0f;
	glGetFloatv(GL_POINT_SIZE_MAX_ARB, &maxSize);
	Log::Info(std::string("Max Point Size:\t") + std::to_string((int)maxSize));
	
	int maxTextures = 0;
	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &maxTextures);
	Log::Info(std::string("Max Textures:\t") + std::to_string(maxTextures));

	Log::Info("--------------------------------------------------------------------------");

	draw.LoadStartupData();

	pGraphic = this;
}
void Graphic::Create(SDL_GLContext context, int width, int height)
{
	assert(!bInitialized);
	if (bInitialized)
		return;

	assert(glContext == nullptr);
	glContext = context;

	newSize = PointI(width, height);

	draw.Create();

	// Set Shader projection
	Resize(width, height);

	bInitialized = true;
}
void Graphic::Dispose()
{
	//assert(bInitialized);
	if (!bInitialized)
		return;

	draw.Dispose();

	if (glContext)
	{
		Log::Write("deleting gl context");
		SDL_GL_DeleteContext(glContext);
		glContext = nullptr;
	}

	bInitialized = false;
}

Graphic::~Graphic()
{
	
}

float scalar = 1.0f;

void Graphic::Resize(int width, int height)
{
	newSize = PointI(width, height);
	
	// set parameters in framework
	float scaleX = float(width) / float(Framework::STD_DRAW_X);
	float scaleY = float(height) / float(Framework::STD_DRAW_Y);

	scalar = std::min(scaleX, scaleY);

	if (scaleY > scaleX)
	{
		scaleY /= scaleX;
		scaleX = 1.0f;
	}
	else
	{
		scaleX /= scaleY;
		scaleY = 1.0f;
	}

	newWidth = float(Framework::STD_DRAW_X) * scaleX;
	newHeight = float(Framework::STD_DRAW_Y) * scaleY;

	aspect = newWidth / newHeight;
	fovy = 2 * atanf(float(newHeight / 2) / 10.0f);

	Framework::SetWindowSize(width, height, (int)newWidth, (int)newHeight, scalar);

	bResizeQueued = true;
}
void Graphic::ApplyResize()
{
	bResizeQueued = false;
	curWidth = (float)newSize.x;
	curHeight = (float)newSize.y;
	

	glm::mat4 projection = glm::perspective(fovy, aspect, 0.1f, 100.0f);

	static const PointF midpoint(Framework::STD_DRAW_X / 2, Framework::STD_DRAW_Y / 2);
	// camera
	glm::mat4 cam = glm::lookAt(
		glm::vec3{ midpoint.x, midpoint.y, -10.0f }, // camera
		glm::vec3{ midpoint.x, midpoint.y, 0.0f }, // look at
		glm::vec3{ 0.0f, -1.0f, 0.0f }); // upvektor

	draw.SetPerspective(projection * cam);
	draw.SetMetrics(newWidth, newHeight, (int)curWidth, (int)curHeight);

	curProj = projection * cam;

	glViewport(0, 0, (GLsizei)curWidth, (GLsizei)curHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(&curProj[0][0]);

	GLCheck("projection");
}

void Graphic::BeginFrame()
{
	/*glEnable(GL_MULTISAMPLE);
	GLCheck("multisample");*/

	if (bResizeQueued)
		ApplyResize();

	draw.PrepareImageFilter();

	// reload matrices
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(&curProj[0][0]);

	GLCheck("projection");

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	draw.DrawParticle({ 10, 10 }, Color::Black(), 10.0f);

	draw.DrawBackground();
	draw.DrawStarfield();
}

void Graphic::EndFrame()
{
	
	GLCheck("EndFrame");
	
	draw.ApplyPostProcessing();

	glFlush();
	GLCheck("Image Filter");
}