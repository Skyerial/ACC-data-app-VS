#pragma once

#include "nlohmann/json.hpp"
#include <string>

using json = nlohmann::json;

std::string getSessionType(int session);

std::wstring newFile(int session, const std::wstring file_type, const std::wstring mydoc_path);

json readFromFile(std::wstring file_location);

void writeToFile(json& j, std::wstring file_location);
