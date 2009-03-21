#include <SDL/SDL.h>
#include "glheaders.h"
#include "GraphicsDevice.h"

#ifdef _WIN32
void GraphicsDevice::flushMessageQueue() {
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT) break;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
#endif

GraphicsDevice::~GraphicsDevice() {
	SDL_FreeSurface(windowSurface);
	windowSurface = 0;
}

GraphicsDevice::GraphicsDevice(const ivec2 &windowSize,
							   bool fullscreen,
							   bool resizable,
							   bool showcursor,
							   const char * title)
: windowSurface(0),
  dimensions(0,0),
  nearClip(0.1f),
  farClip(100.0f) {
	attr = generateDefaultAttributes();
	
	setAttributes();
	
	if(title) {
		SDL_WM_SetCaption(title, title);
	} else {
		SDL_WM_SetCaption("SDL App", "SDL App");
	}
	
	windowSurface = SDL_SetVideoMode(windowSize.x,
	                                 windowSize.y,
	                                 0,
	                                 SDL_OPENGL
	                                 | SDL_HWSURFACE
	                                 | SDL_DOUBLEBUF
									 | (resizable?SDL_RESIZABLE:0)
	                                 | (fullscreen?SDL_FULLSCREEN:0));
	                                
#ifdef _WIN32
	/*
	See:
	<http://listas.apesol.org/pipermail/sdl-libsdl.org/2002-July/028824.html>
	Sometimes, after calling SDL_SetVideoMode, MessageBox and standard assert
	(which calls MessageBox internally) will return immediately without
	displaying a message box window.
	*/
	flushMessageQueue();
#endif
	
	assert(windowSurface && "Couldn't create window!");
	
	initializeOpenGLExtensions();
	resizeOpenGLViewport(windowSize, nearClip, farClip);
	
	SDL_ShowCursor(showcursor ? SDL_ENABLE : SDL_DISABLE);
}

GraphicsDevice::GL_ATTRIBUTES GraphicsDevice::generateDefaultAttributes() {
	int red = 8;
	int green = 8;
	int blue = 8;
	int alpha = 8;
	bool doublebuffer = true;
	int depth = 24;
	int fsaaBuffers = 0; // FSAA is not available on Intel X3100
	int fsaaSamples = 0;
	
	GL_ATTRIBUTES attr;
	
	attr.insert(std::make_pair(SDL_GL_RED_SIZE,           red));
	attr.insert(std::make_pair(SDL_GL_GREEN_SIZE,         green));
	attr.insert(std::make_pair(SDL_GL_BLUE_SIZE,          blue));
	attr.insert(std::make_pair(SDL_GL_ALPHA_SIZE,         alpha));
	attr.insert(std::make_pair(SDL_GL_DOUBLEBUFFER,       doublebuffer?1:0));
	attr.insert(std::make_pair(SDL_GL_DEPTH_SIZE,         depth));
	attr.insert(std::make_pair(SDL_GL_MULTISAMPLEBUFFERS, fsaaBuffers));
	attr.insert(std::make_pair(SDL_GL_MULTISAMPLESAMPLES, fsaaSamples));
	
	return attr;
}

void GraphicsDevice::setAttribute(std::pair<SDL_GLattr, int> attribute) {
	SDL_GL_SetAttribute(attribute.first, attribute.second);
}

void GraphicsDevice::setAttributes() {
	for_each(attr.begin(), attr.end(), GraphicsDevice::setAttribute);
}

Mat4 GraphicsDevice::getProjectionMatrix() {
	Mat4 m;
	glGetDoublev(GL_PROJECTION_MATRIX, m.m);
	return m;
}

Mat4 GraphicsDevice::getModelViewMatrix() {
	Mat4 m;
	glGetDoublev(GL_MODELVIEW_MATRIX, m.m);
	return m;
}

Mat3 GraphicsDevice::getCameraOrientation() {
	const Mat4 modl = getModelViewMatrix();
	Mat3 orientation;

	memcpy(orientation._m[0], modl._m[0], sizeof(real_t)*3);
	memcpy(orientation._m[1], modl._m[1], sizeof(real_t)*3);
	memcpy(orientation._m[2], modl._m[2], sizeof(real_t)*3);

	return orientation;
}

void GraphicsDevice::initializeOpenGLExtensions() {
	GLenum err = glewInit();
	if(GLEW_OK != err) {
		std::cerr << (const char*)glewGetErrorString(err) << std::endl;
	}
}

void GraphicsDevice::resizeOpenGLViewport(const ivec2 &_dimensions,
  float _nearClip,
  float _farClip) {
	assert(_dimensions.x!=0 && "Window width equals zero");
	assert(_dimensions.y!=0 && "Window height equals zero");
	assert(_nearClip > 0 && "Near clip plan is too close");
	assert(_farClip > _nearClip && "Far clip plane is too close");
	
	dimensions = _dimensions;
	nearClip = _nearClip;
	farClip = _farClip;
	
	const float aspectRatio = (GLfloat)dimensions.x/(GLfloat)dimensions.y;
	
	// Reset view port
	glViewport(0, 0, dimensions.x, dimensions.y);
	
	// Reset projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, aspectRatio, nearClip, farClip);
	
	// Reset model-view matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
