/**
* @file searchfile.cpp
* @brief Searches for files following a certain pattern.
* @author Andrew Fox (arfox)
*/

#ifdef _WIN32
#include <io.h>
#else
#include <dirent.h>
#endif

#include <iostream>
#include <errno.h>

#include "searchfile.h"

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
static size_t findExtensionDelimeter(const std::string &fileName)
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

/**
Gets the file extension from a file path
@param fileName The name of the file to extract the extension from
@return Returns the file extension
*/
static std::string getFileExtension(const std::string &fileName)
{
	return fileName.substr(findExtensionDelimeter(fileName), fileName.length());
}

std::vector<std::string> SearchFile(const std::string &_searchDirectory,
                            const std::string &fileExtension) {
	std::vector<std::string> filesFound;
	std::string searchDirectory;
	
#ifdef _WIN32
	searchDirectory = _searchDirectory + "/*" + fileExtension;
	
	// Search for all files matching the pattern
	struct _finddata_t file;
	long hFile = (long)_findfirst(searchDirectory.c_str(), &file);
	
	if (hFile != -1L) {
		do {
			if (getFileExtension(file.name) == fileExtension) {
				filesFound.push_back(file.name);
			}
		} while (_findnext( hFile, &file ) == 0);
		
		// We are done with the handle
		_findclose(hFile);
	} else {
		switch (errno) {
		case EINVAL:
			std::cerr << "Invalid parameter: filespec or fileinfo was NULL. Or, the operating system returned an unexpected error." << std::endl;
			break;
		case ENOENT:
			std::cerr << "File specification that could not be matched." << std::endl;
			break;
		case ENOMEM:
			std::cerr << "Insufficient memory." << std::endl;
			break;
		default:
			std::cerr << "Unknown error" << std::endl;
			break;
		};
	}
#else
	searchDirectory = _searchDirectory;
	struct dirent *directoryEntry = 0;
	DIR *directory = opendir(searchDirectory.c_str());
	
	while ((directoryEntry = readdir(directory)) != 0) {
		FileName name(directoryEntry->d_name);
	
		if (name.getExtension() == fileExtension) {
			filesFound.push_back(name);
		}
	}
	
	closedir(directory);
#endif
	
	return filesFound;
}
