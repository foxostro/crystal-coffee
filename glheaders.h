/**
 * @file glheaders.h
 * @brief Includes the correct opengl and glut header files depending on the
 *  platform. Use this file to include any gl header files.
 */

#ifndef _GLHEADERS_H_
#define _GLHEADERS_H_

#define GL_GLEXT_PROTOTYPES

#ifdef _OS_X_
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else

// fixes bizarre header errors with windows.h
#ifdef _WIN32
#define NOMINMAX
#include <iostream>
#include <GL/glew.h>
#endif /* WIN32 */

#include <cstdlib>
#include <GL/glut.h>
#include <GL/glext.h>
#endif /* _OS_X_ */

#endif /* _GLHEADERS_H_ */

