/**
* @file devil_wrapper.h
* @brief Wrapper for the DevIL image library.
*
* @author Andrew Fox (arfox)
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _DEVIL_WRAPPER_H_
#define _DEVIL_WRAPPER_H_

#include <string>

/* DevIL Image Library */
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

/** Initializes DevIL */
void devil_init(void);

/**
Loads an image and returns the DevIL image handle
@param fileName File name of the image
@return DevIL image handle
*/
unsigned int devil_loadImage(const std::string &fileName);

/**
Loads an image and returns the DevIL image handle
@param fileName File name of the image
@return DevIL image handle
*/
unsigned int devil_loadImage(const char * fileName);

/**
Calls DevIL's ilLoadImage, but allowing FileName type parameter
@param fileName File name of the image
@return DevIL image handle
*/
ILboolean ilLoadImage(const std::string &fileName);

#endif
