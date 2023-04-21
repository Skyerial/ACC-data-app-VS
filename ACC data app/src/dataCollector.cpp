// The first include is needed for the precompiled header file which makes 
// sure that all the cpp files that have includes in this header file as well 
// dont need to have their header files compiled again. This in the end brings a
// speed boost to compiling.
#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include <string>

// use "" for header files in project folder, <> is used for headers that are 
// outside of project folder
#include "dataUITransfer.h"
#include "dataCollector.h"
#include "dataInitDismiss.h"
#include "dataToFile.h"
#include "lapData.h"
#include "sessionData.h"
#include "SharedFileOut.h"

#include "nlohmann/json.hpp"

using json = nlohmann::json;

//#pragma optimize("",off)

// should this be here?...
//template <typename T, unsigned S>
//inline unsigned arraysize(const T(&v)[S])
//{
//	return S;
//}

// pair is the struct that is used to transfer data between ui and data to show
// that data is being gathered when a session is active
void dataCollector(std::wstring mydoc_path, ui_data_pair& pair)
{
	initPhysics();
	initGraphics();
	initStatic();

	SPageFileGraphic* pfg = (SPageFileGraphic*)m_graphics.mapFileBuffer;
	SPageFilePhysics* pfp = (SPageFilePhysics*)m_physics.mapFileBuffer;
	SPageFileStatic* pfs = (SPageFileStatic*)m_static.mapFileBuffer;

	// initialize session_check
	AC_SESSION_TYPE lastSessionType = AC_UNKNOWN;
	bool newSession = true;

	while(true) {
		std::vector<LapData> laps = createLaps();
		SessionData session;
		// 2 means session is running, 3 session running but in esc menu
		// this also means nothing happens when neither is true since it returns 0
		// at the end
		while (pfg->status == 2 || pfg->status == 3)
		{
			if (newSession) {
				newSession = false;
				pair.session_active = true;
				lastSessionType = pfg->session;
				session.setSessionData(pfs, pfg);
				//std::cout << "new session started" << std::endl;
			}
			// break out of session loop when a session changes
			if (lastSessionType != pfg->session) {
				break;
			}
			// if session running create file and record session type(need to check
			// also if new session type starts), time of day at start, car model,
			// track and multiplayer or singleplayer
			// std::cout << pfg->iLastTime << '\n';
			laps = updateLap(pfg, laps);
			pair.lapnumber = laps.back().getLapNumber(); // this feels slightly ulgly but oh well
			Sleep(100); // in miliseconds, tickrate is 10Hz
		}

		if (!newSession) {
			newSession = true;
			pair.session_active = false;
			std::wstring file_location = newFile(lastSessionType, L".json", mydoc_path);
			json jLaps = laps;
			json jSession = session;
			jSession["Laps"] = jLaps;
			writeToFile(jSession, file_location);
		}
	}

	// end of main, needs to be made to run as long as the application is open.
	dismissAll();
	return;
}
