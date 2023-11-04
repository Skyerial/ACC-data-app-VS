//////////////////////////////////////////////////////////////////////////////
// Author: Daniel Oppenhuizen
// License: None
// 
// Spawn different forms of collecting data when a session starts.
// Store data from all forms in database together.
//////////////////////////////////////////////////////////////////////////////

#include "Collectors.h"

#include <thread>

#include "dataInitDismiss.h"
#include "SharedFileOut.h"
#include "dataCollector.h"
#include "socket/UdpDataCollector.h"
#include "sessionData.h"
#include "lapData.h"

void collectors(ui_data_pair& pair)
{
	initGraphics();
	initPhysics();
	initStatic();

	SPageFileGraphic* pfg = (SPageFileGraphic*)m_graphics.mapFileBuffer;
	SPageFilePhysics* pfp = (SPageFilePhysics*)m_physics.mapFileBuffer;
	SPageFileStatic* pfs = (SPageFileStatic*)m_static.mapFileBuffer;

	while (pair.UI_running)
	{
		while (pfg->status == 2 || pfg->status == 3)
		{
			std::vector<LapData> data_laps = CreateLaps();
			SessionData data_session;

			std::vector<LapData> udp_laps = CreateLaps();
			SessionData udp_session;

			std::jthread data_thread(DataCollector, std::ref(pair), std::ref(pfg), std::ref(pfp), std::ref(pfs),
									 std::ref(data_laps), std::ref(data_session));
			std::jthread udp_thread(UdpDataCollector, std::ref(pair), std::ref(udp_laps), std::ref(udp_session));

			data_thread.join();
			udp_thread.join();

			// add the session and lap together
			

			// add the session and lap to database
			//	if (laps.size() > 1)
			//	{
			//		if (laps.back().getLapNumber() == laps[laps.size() - 2].getLapNumber())
			//			laps.pop_back();

			//		InsertSessionLap(session, laps);
			//	}
		}
	}

	dismissAll();
}
