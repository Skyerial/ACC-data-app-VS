//////////////////////////////////////////////////////////////////////////////
// Author: Daniel Oppenhuizen
// License: None
// 
// Implementation of the SessionData class functions.
//////////////////////////////////////////////////////////////////////////////
#include "sessionData.h"

#include "iostream" // wcout, 

#include "SharedFileOut.h"
#include "lapData.h"

namespace
{
std::wstring GetDateTime()
{
	time_t rawtime;
	struct tm timeinfo;
	wchar_t buffer[20];

	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);

	wcsftime(buffer, 20, L"%Y-%m-%d %H:%M:%S", &timeinfo);

	std::wcout << buffer << std::endl; //TEST

	return buffer;
}
} // anonymous namespace

void SessionData::SetSessionData(SPageFileStatic* pfs, SPageFileGraphic* pfg)
{
	date = GetDateTime();
    session = pfg->session;
    circuit = pfs->track;
    car = pfs->carModel;
	online = pfs->isOnline;
	race_number = pfg->playerCarID;
	driver_name = pfs->playerName;
	driver_surname = pfs->playerSurname;
}

// This function is slightly useless still
// could also make this a helper function and let sessionData have
// only one setter function
void SessionData::SetBestLap(std::vector<LapData>& laps) {
	best_lap = laps[0].getLapTime();

	for (size_t i = 1; i < laps.size(); i++)
	{
		if (laps[i].getLapTime() < best_lap)
			best_lap = laps[i].getLapTime();
	}
}

void SessionData::Print() {
    printf("%ls\n%ls\n%d\n", circuit, car, best_lap);
}