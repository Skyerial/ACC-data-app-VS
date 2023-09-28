//////////////////////////////////////////////////////////////////////////////
// Author: Daniel Oppenhuizen
// License: None
// 
// This file is responsible for listening in on the shared memory and
// collecting all the right data from it. This then gets stored in a file that
// can be used by dataAppUI.
//////////////////////////////////////////////////////////////////////////////

// .h of this .cpp
#include "stdafx.h"
#include "dataCollector.h"

// C system headers
#include <windows.h>

// Own .h
// use "" for header files in project folder, <> is used for headers that are 
// outside of project folder
#include "lapData.h"
#include "dataInitDismiss.h"
#include "sessionData.h"
#include "SharedFileOut.h"
#include "database/database.h"
#include "dataUITransfer.h" // ui_data_pair

//#pragma optimize("",off)

// should this be here?...
//template <typename T, unsigned S>
//inline unsigned arraysize(const T(&v)[S])
//{
//	return S;
//}

// pair is the struct that is used to transfer data between ui and data to show
// that data is being gathered when a session is active
void DataCollector(ui_data_pair& pair)
{
	initPhysics();
	initGraphics();
	initStatic();

	SPageFileGraphic* pfg = (SPageFileGraphic*)m_graphics.mapFileBuffer;
	SPageFilePhysics* pfp = (SPageFilePhysics*)m_physics.mapFileBuffer;
	SPageFileStatic* pfs = (SPageFileStatic*)m_static.mapFileBuffer;

	// initialize session_check
	AC_SESSION_TYPE last_session_type = AC_UNKNOWN;
	bool new_session = true;

	while(pair.UI_running) { 
		std::vector<LapData> laps = CreateLaps();
		SessionData session;
		// 2 means session is running, 3 session running but in esc menu
		// this also means nothing happens when neither is true since it returns 0
		// at the end
		while (pfg->status == 2 || pfg->status == 3)
		{
			if (new_session) {
				new_session = false;
				pair.session_active = true;
				last_session_type = pfg->session;
				session.Update(pfs, pfg);
				//std::cout << "new session started" << std::endl;
			}
			// break out of session loop when a session changes
			if (last_session_type != pfg->session) {
				break;
			}
			// if session running create file and record session type(need to check
			// also if new session type starts), time of day at start, car model,
			// track and multiplayer or singleplayer
			UpdateLap(pfg, laps);
			session.BestLap(laps.back().getLapTime());

			pair.lapnumber = laps.back().getLapNumber(); // this feels slightly ulgly but oh well
			pair.in_pit = pfg->isInPitLane;
			
			Sleep(100); // in miliseconds, tickrate is 10Hz
		}

		if (!new_session) {
			new_session = true;
			pair.session_active = false;

			if (laps.size() > 1)
			{
				if (laps.back().getLapNumber() == laps[laps.size() - 2].getLapNumber())
					laps.pop_back();

				// write to database TEST
				InsertSessionLap(session, laps);
			}
		}

		Sleep(100);
	}

	// end of main, needs to be made to run as long as the application is open.
	dismissAll();
	return;
}
