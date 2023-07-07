#pragma once
#include "sessionData.h"
#include "lapData.h"

int InsertSessionLap(SessionData& session, std::vector<LapData>& laps);

void RetrieveSession(std::vector<SessionData>& session, int limit, int offset);

void RetrieveLaps(std::vector<LapData>& laps, int session_id);

void MakeDB();
