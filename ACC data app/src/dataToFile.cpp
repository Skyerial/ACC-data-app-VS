//////////////////////////////////////////////////////////////////////////////
// Author: Daniel Oppenhuizen
// License: None
// 
// This file is responsible for creating files with the right name, opening 
// files and parsing its input so the program can use it. Furthermore it also
// stores the data to the file.
//////////////////////////////////////////////////////////////////////////////

#include "dataToFile.h"

#include <fstream>
#include <iostream>

#include "nlohmann\json.hpp"

#include "stdafx.h"
#include "lapData.h"

using json = nlohmann::json;

std::string GetSessionType(int session) {
	std::string sessionType;
	switch (session) {
	case -1: sessionType = "UNKOWN"; break;
	case 0: sessionType = "practise"; break;
	case 1: sessionType = "qualy"; break;
	case 2: sessionType = "race"; break;
	case 3: sessionType = "hotlap"; break;
	case 4: sessionType = "timeattack"; break;
	case 5: sessionType = "drift"; break;
	case 6: sessionType = "drag"; break;
	case 7: sessionType = "hotstint"; break;
	case 8: sessionType = "hotlapsuperpole"; break;
	default:
		break;
	}

	return sessionType;
}

// Helper function that gets the the current date and time and returns it as a std::wstring
// Static since it is a helper function
// This should also make it local to this translation unit
static std::wstring GetTime()
{
	time_t rawtime;
	struct tm timeinfo;
	wchar_t buffer[20];

	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);

	wcsftime(buffer, 20, L"%Y%m%d%H%M%S", &timeinfo);

	return buffer;
}

// creates the path for the new file, but doesnt open a stream to that location
std::wstring NewFile(int session, const std::wstring file_type, const std::wstring mydoc_path)
{
	std::wstring mydoc_cpy;
	mydoc_cpy.append(mydoc_path);
	mydoc_cpy.append(L"\\ACC app data\\");
	mydoc_cpy.append(GetTime());
	mydoc_cpy.append(file_type);

#ifdef DEBUG
	std::wcout << mydoc_cpy << std::endl;
#endif // DEBUG

	return mydoc_cpy;
}

json ReadFromFile(std::wstring file_location) {
	std::ifstream infile;
	infile.open(file_location);
	if (!infile.is_open())
	{
		fprintf(stderr, "couldn't open file\n");
		return NULL;
	}
	std::string file_contents((std::istreambuf_iterator<char>(infile)), (std::istreambuf_iterator<char>()));

	json j = json::parse(file_contents);
	infile.close();

	return j;
}

void WriteToFile(json& j, std::wstring file_location) {
	std::ofstream outfile;
	outfile.open(file_location);
	if (!outfile.is_open())
	{
		fprintf(stderr, "couldn't open file\n");
		return;
	}
	outfile << j << "\n";
	outfile.close();
}
