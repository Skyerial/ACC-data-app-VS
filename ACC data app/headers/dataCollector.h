#pragma once
#include "dataUITransfer.h"
#include "sessionData.h"
#include "lapData.h"

// This function is responsible for keeping track of the game session and changes
// that happen. When sessions end a file is created to store the vector with the
// laps and session data.
// Input:
//	mydoc_path = string that contains path to the my documents folder
//	pair       = struct that is given to the collector and UI to communicate
//				the state of the game so that the live window can be updated 
//				accordingly.
void DataCollector(ui_data_pair& pair, SPageFileGraphic*& pfg, SPageFilePhysics*& pfp, SPageFileStatic*& pfs,
				   std::vector<LapData>& laps, SessionData& session);
