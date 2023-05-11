#pragma once
#include "SharedFileOut.h"
#include "nlohmann\json.hpp"
#include <vector>

using json = nlohmann::json;

// This class is used to keep all the data of lap together. So the intend is
// to create a new object for every lap and make sure all the data of that lap is
// stored in that object.
//
// It also has functions to change the values of the lap object when new 
// information for that lap becomes available.
class LapData
{
private:
    int currentLap;
    int validLap;       // not implemented yet
    int laptime;        // in miliseconds
    int sector1;        // ^
    int sector2;        // ^
    int sector3;        // ^

public:
    // constructor
    LapData() 
    {
        currentLap = 0;
        validLap = 0;
        laptime = 0;
        sector1 = 0;
        sector2 = 0;
        sector3 = 0;
    };

    int getLapNumber() { return currentLap; }
    int getLapTime() { return laptime; }
    int getSector1() { return sector1; }
    int getSector2() { return sector2; }
    int getSector3() { return sector3; }

    void SetLapNumber(SPageFileGraphic* pfg);
    void SetLapTimeValid(SPageFileGraphic* pfg);
    void AddCurrentSectorTime(SPageFileGraphic* pfg);

    // JSON stuff
    friend void to_json(json& j, const LapData& ld) {
        j["current lap"] = ld.currentLap;
        j["laptime"] = ld.laptime;
        j["sector1"] = ld.sector1;
        j["sector2"] = ld.sector2;
        j["sector3"] = ld.sector3;
    }

    // TESTING
    void Print();
};

std::vector<LapData> CreateLaps();

// UpdateLap gets the right memory location to check for data that is available.
// Once it sees that there is new information is available for the given lap
// it updates that lap.
void UpdateLap(SPageFileGraphic* pfg, std::vector<LapData>& laps);
