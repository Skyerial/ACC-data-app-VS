#include "lapData.h"

#include <memory>
#include <vector>
#include <iostream>

#include "SharedFileOut.h"
#include "dataToFile.h"

void LapData::SetLapNumber(SPageFileGraphic* pfg) {
    currentLap = pfg->completedLaps;
}

void LapData::SetLapTimeValid(SPageFileGraphic* pfg) {
    validLap = pfg->isValidLap; // this needs to be done with the UDP connection thing...
    laptime = pfg->iLastTime;
}

void LapData::AddCurrentSectorTime(SPageFileGraphic* pfg) {
    switch (pfg->currentSectorIndex) 
    {
        case 1:
            sector1 = pfg->lastSectorTime;
            break;
        case 2:
            sector2 = pfg->lastSectorTime - sector1;
            break;
        case 0:
            sector3 = laptime - sector1 - sector2;
            break;
    }
}

void LapData::Print() {
    printf("%d\n\t%d\n\t%d\n\t%d\n\t%d\n", currentLap, laptime, sector1, sector2, sector3);
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
        LapData current;
        current.SetLapNumber(pfg);
        laps.push_back(current);
    } else {
        laps.back().AddCurrentSectorTime(pfg);
    }
}
