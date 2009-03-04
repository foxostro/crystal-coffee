/**
* @file searchfile.h
* @brief Searches for files following a certain pattern.
* @author Andrew Fox (arfox)
*/

#ifndef _SEARCH_FILE_H_
#define _SEARCH_FILE_H_

#include <string>
#include <vector>

/** Searches for files in the current directory matching a pattern.
 *  @param searchDirectory Directory to search within
 *  @param fileExtension File extension to search for
 *  @return List of files found
 */
std::vector<std::string> SearchFile(const std::string &searchDirectory,
									const std::string &fileExtension);

#endif
