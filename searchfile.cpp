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

#include "string_helper.h"
#include "searchfile.h"

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
		std::string name(directoryEntry->d_name);
	
		if (getFileExtension(name) == fileExtension) {
			filesFound.push_back(name);
		}
	}
	
	closedir(directory);
#endif
	
	return filesFound;
}
