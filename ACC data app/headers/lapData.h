#pragma once
#include "SharedFileOut.h"
#include "nlohmann\json.hpp"
#include <vector>

using json = nlohmann::json;

class LapData
{
private:
    int currentLap;
    int validLap;
    wchar_t* wLapTime;
    int laptime;        // in miliseconds
    int sector1;        // ^
    int sector2;        // ^
    int sector3;        // ^

public:
    int getLapNumber() { return currentLap; }
    int getLapTime() { return laptime; }
    int getSector1() { return sector1; }
    int getSector2() { return sector2; }
    int getSector3() { return sector3; }

    void setLapNumber(SPageFileGraphic* pfg);
    void setLapTimeValid(SPageFileGraphic* pfg);
    void addCurrentSectorTime(SPageFileGraphic* pfg);

    // JSON stuff
    friend void to_json(json& j, const LapData& ld) {
        j["current lap: "] = ld.currentLap;
        j["laptime: "] = ld.laptime;
        j["sector1: "] = ld.sector1;
        j["sector2: "] = ld.sector2;
        j["sector3: "] = ld.sector3;
    }

    // TESTING
    void print();
};

std::vector<LapData> createLaps();

std::vector<LapData> updateLap(SPageFileGraphic* pfg, std::vector<LapData> laps);
