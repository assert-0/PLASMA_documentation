#pragma once

#include <string>
#include <vector>

//Loads a file and saves it to the byte vector.
bool loadFromFileToBuffer(std::string path, std::vector<char> &data);

//Writes the contents of the byte vector to a file.
void dumpToFileFromVuffer(std::string path, const std::vector<char> &data);

//Launches explorer window and returns selected files.
void getFileFromExplorer(std::vector<std::string> &results);
