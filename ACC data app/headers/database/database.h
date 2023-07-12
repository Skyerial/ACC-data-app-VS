#pragma once
#include "sessionData.h"
#include "lapData.h"

int InsertSessionLap(SessionData& session, std::vector<LapData>& laps);

void RetrieveSession(std::vector<SessionData>& session, int limit, int offset);

void RetrieveLaps(std::vector<LapData>& laps, int session_id);

// Session and corresponding laps will be deleted based on given session_id
// Deleted session_id wont return when new session gets added
void DeleteSessionLaps(const int session_id);

void ResetTable();
