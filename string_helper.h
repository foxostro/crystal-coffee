/**
* @file string_helper.h
* @brief Misc. helper functions for STL strings.
* @author Andrew Fox (arfox)
*/

#ifndef _STRING_HELPER_H_
#define _STRING_HELPER_H_

#include <string>

/**
Finds the extension marker in a string.
findExtensionDelimeter determines the index of the extension delimiter
character in a given fileName, when possible. For example, calling with
the fileName = "image.jpeg" will return 5 and "data/text.txt" will
return 9.  Also be aware that "data/text.txt.bak" will return 13, as
this is the last string that is clearly identifiable as a file
extension.
@param fileName Name of the file
@return Index of the extension delimiter character in the fileName.
If there is no extension, then the length of the filename is returned.
*/
size_t findExtensionDelimeter(const std::string &fileName);

/**
Gets the file extension from a file path
@param fileName The name of the file to extract the extension from
@return Returns the file extension
*/
std::string getFileExtension(const std::string &fileName);

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
