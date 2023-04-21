#include "stdafx.h"
#include "dataToFile.h"
#include "nlohmann\json.hpp"
#include "lapData.h"

#include <fstream>

// test
#include <iostream>

using json = nlohmann::json;

std::string getSessionType(int session) {
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
std::wstring getTime()
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
std::wstring newFile(int session, const std::wstring file_type, const std::wstring mydoc_path)
{
	std::wstring mydoc_cpy;
	mydoc_cpy.append(mydoc_path);
	mydoc_cpy.append(L"\\ACC app data\\");
	mydoc_cpy.append(getTime());
	mydoc_cpy.append(file_type);

	// testing line
	std::wcout << mydoc_cpy << std::endl;

	return mydoc_cpy;
}

json readFromFile(std::wstring file_location) {
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

void writeToFile(json& j, std::wstring file_location) {
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
