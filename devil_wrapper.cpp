/**
* @file devil_wrapper.cpp
* @brief Wrapper for the DevIL image library.
*
* @author Andrew Fox (arfox)
*/

#include <iostream>

#include "string_helper.h"
#include "devil_wrapper.h"

void devil_init()
{
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
	ilutEnable(ILUT_OPENGL_CONV);
}

ILboolean ilLoadImage(const std::string &fileName) {
	return ::ilLoadImage(fileName.c_str());
}

unsigned int devil_loadImage(const std::string &fileName) {
	unsigned int imageName = 0;
	ilGenImages(1, &imageName);
	ilBindImage(imageName);
	
	ilLoadImage(fileName);
	
	ILenum error = ilGetError();
	
	if (error != IL_NO_ERROR) {
		ILstring errstring = iluErrorString(error);
		std::cerr << "DevIL error: " << errstring << std::endl;
		return 0;
	} else {
		if (ilGetInteger(IL_ORIGIN_MODE) == IL_ORIGIN_LOWER_LEFT &&
		    getFileExtension(fileName) != ".tif") {
			iluFlipImage();
		}
		
		return imageName;
	}
}
