#ifndef _WIN32
#include <boost/lexical_cast.hpp>
#endif

#include <stdexcept>
#include <cmath>
#include "string_helper.h"

size_t findExtensionDelimeter(const std::string &fileName)
{
	for (size_t i=0; i<fileName.length(); ++i) {
		char c = fileName[fileName.length() - i - 1];

		if (c == '.') {
			// Found the delimiter, return its index
			return fileName.length() - i - 1;
		}

		if (c == '\\' || c == '/') {
			// Found that there was no delimiter, return the index of the end of the string
			return fileName.length();
		}
	}

	// Found that there was no delimiter, return the index of the end of the string
	return fileName.length();
}

std::string getFileExtension(const std::string &fileName)
{
	return fileName.substr(findExtensionDelimeter(fileName), fileName.length());
}

int stoi(const std::string &s) {
	if (s.empty())
		throw std::runtime_error("stoi: cannot convert an empty string");

	return atoi(s.c_str());
}

std::string itos(int i) {
#ifdef _WIN32
	char buffer[64] = {0};
	_itoa_s(i, buffer, 64, 10);
	return std::string(buffer);
#else
	return boost::lexical_cast<std::string>(i);
#endif
}

std::string justify_string_in_field(const std::string &in,
						            char padWith,
						            size_t fieldSize,
						            JUSTIFY justify) {
	if (in.size() > fieldSize) {
		return in.substr(0, fieldSize);
	} else if (in.size() == fieldSize) {
		return in;
	} else {
		size_t charsRemaining = fieldSize-in.size();
		size_t leftCharsRemaining = (size_t)floor((fieldSize-in.size()) / 2.0);
		size_t rightCharsRemaining = (size_t)ceil((fieldSize-in.size()) / 2.0);
		
		std::string pad;
		for (size_t i=0; i<charsRemaining; ++i) pad += padWith;
		std::string padLeftHalf;
		for (size_t i=0; i<leftCharsRemaining; ++i) padLeftHalf += padWith;
		std::string padRightHalf;
		for (size_t i=0; i<rightCharsRemaining; ++i) padRightHalf += padWith;
		
		switch (justify) {
		case JUSTIFY_CENTER:
			return padLeftHalf + in + padRightHalf;
		case JUSTIFY_RIGHT:
			return pad + in;
		case JUSTIFY_LEFT:
		default:
			return in + pad;
		};
	}
}
