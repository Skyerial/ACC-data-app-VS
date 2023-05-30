#pragma once
#include "sessionData.h"
#include "lapData.h"

void InsertSessionLap(SessionData& session, std::vector<LapData>& laps);

void MakeDB();
