#pragma once

#include "nlohmann/json.hpp"
#include <string>

using json = nlohmann::json;

void openFile(std::wstring file_location);

void closeFile();

void writeToFile(json& j);

void printToFile(std::string name, float value);

template <typename T, unsigned S>
inline void printToFile(const std::string name, const T(&v)[S]);

template <typename T, unsigned S>
inline std::string actualValue(const T(&v)[S]);
