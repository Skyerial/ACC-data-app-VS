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

SessionData::SessionData()
{
	id_ = 0;
	date_ = L"0/0/0 00-00-00";
	session_ = -1;
	best_lap_ = 0;
	circuit_ = L"";
	car_ = L"";
	online_ = 0;
	race_number_ = 0;
	driver_name_ = L"";
	driver_surname_ = L"";
}

SessionData::SessionData(int id, std::wstring date, AC_SESSION_TYPE session, int best_lap,
						std::wstring circuit, std::wstring car, int online, int race_number,
						std::wstring driver_name, std::wstring driver_surname)
{
	id_ = id;
	date_ = date;
	session_ = session;
	best_lap_ = best_lap;
	circuit_ = circuit;
	car_ = car;
	online_ = online;
	race_number_ = race_number;
	driver_name_ = driver_name;
	driver_surname_ = driver_surname;
}

void SessionData::Update(SPageFileStatic* pfs, SPageFileGraphic* pfg)
{
	id_ = 0;
	date_ = GetDateTime();
	session_ = pfg->session;
	best_lap_ = 0;
	circuit_ = pfs->track;
	car_ = pfs->carModel;
	online_ = pfs->isOnline;
	race_number_ = pfg->playerCarID;
	driver_name_ = pfs->playerName;
	driver_surname_ = pfs->playerSurname;
}

void SessionData::BestLap(int laptime)
{
	if (best_lap_ == 0 && laptime > 0)
	{
		best_lap_ = laptime;
	} 
	else if (laptime < best_lap_ && laptime > 0)
	{
		best_lap_ = laptime;
	}
}

// This function is slightly useless still
// could also make this a helper function and let sessionData have
// only one setter function
void SessionData::SetBestLap(std::vector<LapData>& laps) {
	best_lap_ = laps[0].getLapTime();

	for (size_t i = 1; i < laps.size(); i++)
	{
		if (laps[i].getLapTime() < best_lap_)
			best_lap_ = laps[i].getLapTime();
	}
}
