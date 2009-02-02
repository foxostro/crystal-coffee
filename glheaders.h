/**
 * @file glheaders.h
 * @brief Includes the correct opengl and glut header files depending on the
 *  platform. Use this file to include any gl header files.
 */

#ifndef _GLHEADERS_H_
#define _GLHEADERS_H_

/*
   OPTIONAL:
   Comment out this define to prevent the loading and linking of glsl extension
   functions. Use this if you wish to do projects 1,3 and 4 on a machine that
   does not support glsl. Note that you will have to make sure to put all of
   your own code that uses gl extensions inside #ifdef blocks for this to work.
*/
#define USE_GLSL

#ifdef USE_GLSL
#define GL_GLEXT_PROTOTYPES
#endif /* USE_GLSL */

#ifdef _OS_X_
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else

// fixes bizarre header errors with windows.h
#ifdef WIN32
#define NOMINMAX
#include <iostream>
#include <GL/glew.h>
#endif /* WIN32 */

#include <GL/glut.h>
#ifdef USE_GLSL
#include <GL/glext.h>
#endif /* USE_GLSL */
#endif /* _OS_X_ */

#endif /* _GLHEADERS_H_ */

