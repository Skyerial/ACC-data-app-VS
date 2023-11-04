#pragma once
#include "dataUITransfer.h"

#include <vector>

#include "sessionData.h"
#include "lapData.h"

void UdpDataCollector(ui_data_pair& pair, std::vector<LapData>& laps, SessionData& session);
