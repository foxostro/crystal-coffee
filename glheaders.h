/**
 * @file glheaders.h
 * @brief Includes the correct opengl header files depending on the
 *  platform. Use this file to include any gl header files.
 */

#ifndef _GLHEADERS_H_
#define _GLHEADERS_H_

#define GL_GLEXT_PROTOTYPES

// fixes bizarre header errors with windows.h
#ifdef _WIN32
#define NOMINMAX
#include <iostream>
#include <windows.h>
#endif /* WIN32 */



#ifndef _WIN32
#define DebugBreak() asm("int $3")
#endif



#include <GL/glew.h>
#include <cstdlib>
#include <GL/glext.h>

#define MESH_INDEX_FORMAT_IS_UINT 1
#if MESH_INDEX_FORMAT_IS_UINT
#define MESH_INDEX_FORMAT (GLenum)(GL_UNSIGNED_INT)
typedef unsigned int index_t;
#else
#define MESH_INDEX_FORMAT (GLenum)(GL_UNSIGNED_SHORT)
typedef unsigned short index_t;
#endif

#ifndef NDEBUG
#define CHECK_GL_ERROR() \
for(;;) {                                                                  \
    GLenum errorCode;                                                      \
    while( (errorCode=glGetError()) != GL_NO_ERROR)                        \
    {                                                                      \
		std::cerr << __FILE__ << "(" << __LINE__ << "): "                  \
		          << (const char*)gluErrorString(errorCode) << std::endl;  \
		DebugBreak();                                                      \
    }                                                                      \
	break;}
#else
#define CHECK_GL_ERROR() ((void)0)
#endif

#ifndef NDEBUG
#define CHECK_IL_ERROR() \
for(;;) {                                                                  \
    ILenum errorCode;                                                      \
    while( (errorCode=ilGetError()) != IL_NO_ERROR)                        \
    {                                                                      \
		std::cerr << __FILE__ << "(" << __LINE__ << "): "                  \
		          << (const char*)iluErrorString(errorCode) << std::endl;  \
    }                                                                      \
break;}
#else
#define CHECK_IL_ERROR() ((void)0)
#endif

#endif /* _GLHEADERS_H_ */

