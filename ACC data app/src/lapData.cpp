//////////////////////////////////////////////////////////////////////////////
// Author: Daniel Oppenhuizen
// License: None
// 
// Implementation of functions declared in the LapData class. It also contains
// the function that is responsible for updating the in the laps vector if
// dataCollector.cpp asks it to do so.
//////////////////////////////////////////////////////////////////////////////

#include "lapData.h"

#include <memory>
#include <vector>
#include <iostream>

#include "SharedFileOut.h"
#include "dataToFile.h"

LapData::LapData()
{
    currentLap_ = 0;
    validLap_ = 0;
    laptime_ = 0;
    sector1_ = 0;
    sector2_ = 0;
    sector3_ = 0;
    in_pit_ = 0;
    position_ = 0;
}

LapData::LapData(int lapnumber, int valid, int laptime, int sector1,
                int sector2, int sector3, int in_pit, int position)
{
    currentLap_ = lapnumber;
    validLap_ = valid;
    laptime_ = laptime;
    sector1_ = sector1;
    sector2_ = sector2;
    sector3_ = sector3;
    in_pit_ = in_pit;
    position_ = position;
}

void LapData::SetLapNumber(SPageFileGraphic* pfg) {
    currentLap_ = pfg->completedLaps;
}

void LapData::SetLapTimeValid(SPageFileGraphic* pfg) {
    validLap_ = pfg->isValidLap; // this needs to be done with the UDP connection thing...
    laptime_ = pfg->iLastTime;
}

void LapData::AddCurrentSectorTime(SPageFileGraphic* pfg) {
    switch (pfg->currentSectorIndex) 
    {
        case 1:
            sector1_ = pfg->lastSectorTime;
            break;
        case 2:
            sector2_ = pfg->lastSectorTime - sector1_;
            break;
        case 0:
            sector3_ = laptime_ - sector1_ - sector2_;
            break;
    }
}

void LapData::SetInPit(SPageFileGraphic* pfg)
{
    in_pit_ = pfg->isInPitLane;
}

void LapData::SetPosition(SPageFileGraphic* pfg)
{
    position_ = pfg->position;
}

void LapData::Print() {
    printf("%d\n\t%d\n\t%d\n\t%d\n\t%d\n", currentLap_, laptime_, sector1_, sector2_, sector3_);
}

// maybe we need a function that creates the array of laps for the current session
// then we can return this array and send this array to the updateLap function during that session
// for a new session we can then store the array, delete it
// and create a new array if a new session starts
std::vector<LapData> CreateLaps() {
    std::vector<LapData> laps;
    return laps; // this should probs be done with pointers, need to test...
}

// Everytime this function gets called check if we are on a new lap or in a new sector
// and update values accordingly
// atm this functions updates values even if they are already stored, but thats okay for now
void UpdateLap(SPageFileGraphic* pfg, std::vector<LapData>& laps) {
    // if empty we cannot compare yet to last lap
    if (laps.empty()) {
        LapData current;
        laps.push_back(current);
    } else if (pfg->completedLaps - 1 >= laps.back().getLapNumber()) {
        laps.back().SetLapTimeValid(pfg);
        laps.back().AddCurrentSectorTime(pfg);
        laps.back().SetLapNumber(pfg);
        laps.back().SetInPit(pfg);
        laps.back().SetPosition(pfg);
        LapData current;
        current.SetLapNumber(pfg);
        laps.push_back(current);
    } else {
        laps.back().AddCurrentSectorTime(pfg);
    }
}
