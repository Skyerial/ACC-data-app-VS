#pragma once

#include "nlohmann/json.hpp"
#include <string>

using json = nlohmann::json;

std::string GetSessionType(int session);

std::wstring NewFile(int session, const std::wstring file_type, const std::wstring mydoc_path);

json ReadFromFile(std::wstring file_location);

void WriteToFile(json& j, std::wstring file_location);
