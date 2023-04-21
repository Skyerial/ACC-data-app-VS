#pragma once

#include "nlohmann/json.hpp"
#include <string>

using json = nlohmann::json;

std::string getSessionType(int session);

void newFile(int session, const std::wstring file_type, const std::wstring mydoc_path);

void openFile(std::wstring file_location);

void closeFile(std::wstring file_location);

json readFromFile(std::wstring file_location);

void writeToFile(json& j, std::wstring file_location);



void printToFile(std::string name, float value);

template <typename T, unsigned S>
inline void printToFile(const std::string name, const T(&v)[S]);

template <typename T, unsigned S>
inline std::string actualValue(const T(&v)[S]);
