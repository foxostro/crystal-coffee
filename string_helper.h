/**
* @file string_helper.h
* @brief Misc. helper functions for STL strings.
* @author Andrew Fox (arfox)
*/

#ifndef _STRING_HELPER_H_
#define _STRING_HELPER_H_

#include <string>

/** Converts a value from a string to a integer */
int stoi(const std::string &s);

/** Represents some value as a string */
std::string itos(int i);

/** Specifies how a field should be justified in the fitToFieldSize method */
enum JUSTIFY {
	JUSTIFY_LEFT,
	JUSTIFY_RIGHT,
	JUSTIFY_CENTER
};

/** @brief Pads a string and justifies it if it is less than the field size.
 *  If the string larger than the field size, then the string is truncated at
 *  the field length.
 */
std::string justify_string_in_field(const std::string &in,
					                char padWith,
					                size_t fieldSize,
					                JUSTIFY justify);

#endif
