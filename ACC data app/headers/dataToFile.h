#pragma once

#include "nlohmann/json.hpp"
#include <string>

using json = nlohmann::json;

// Gets the correct session type as a string.
std::string GetSessionType(int session);

// Creates the path for the new file with the filename included in the path
// and returns this as a wstring.
std::wstring NewFile(int session, const std::wstring file_type, const std::wstring mydoc_path);

// Reads a json file and parses the file content to nlohmann::json object.
json ReadFromFile(std::wstring file_location);

// Writes a given nlohmann::json object to the given file.
void WriteToFile(json& j, std::wstring file_location);
