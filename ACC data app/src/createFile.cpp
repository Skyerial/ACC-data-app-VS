#include "stdafx.h"
#include "createFile.h"
#include "dataToFile.h"

#include <fstream> // strlen
#include <iostream>
#include <windows.h>
#include <time.h>

// Helper function that gets the the current date and time and returns it as a std::wstring
std::wstring getTime() {
    time_t rawtime;
    struct tm timeinfo;
    wchar_t buffer[20];

    time(&rawtime);
    localtime_s(&timeinfo, &rawtime);

    wcsftime(buffer, 20, L"%Y%m%d%H%M%S", &timeinfo);

    return buffer;
}

// Helper function that gives back a std::wstring version of the current session
const std::wstring findSessionType(int session) {
    std::wstring sessionType = L"";
    switch (session) {
        case -1: sessionType = L"\\UNKOWN"; break;
        case 0: sessionType = L"\\practise"; break;
        case 1: sessionType = L"\\qualy"; break;
        case 2: sessionType = L"\\race"; break;
        case 3: sessionType = L"\\hotlap"; break;
        case 4: sessionType = L"\\timeattack"; break;
        case 5: sessionType = L"\\drift"; break;
        case 6: sessionType = L"\\drag"; break;
        case 7: sessionType = L"\\hotstint"; break;
        case 8: sessionType = L"\\hotlapsuperpole"; break;
    default:
        break;
    }

    return sessionType;
}

void newFile(int session, const std::wstring file_type, const std::wstring mydoc_path) {
    std::wstring mydoc_cpy;
    mydoc_cpy.append(mydoc_path);
    const std::wstring sessionType = findSessionType(session);
    if (sessionType == L"") {
        std::cerr << "not a known session type" << std::endl;
        exit(-1);
    }
    mydoc_cpy.append(L"\\ACC app data\\");
    //mydoc_cpy.append(sessionType);
    mydoc_cpy.append(getTime());
    mydoc_cpy.append(file_type);
	openFile(mydoc_cpy);

    // testing line
    std::wcout << mydoc_cpy << std::endl;
}