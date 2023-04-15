#include "SharedFileOut.h"
#include "lapData.h"
#include "dataToFile.h"

#include <memory>
#include <vector>
#include <iostream>

void LapData::setLapNumber(SPageFileGraphic* pfg) {
    currentLap = pfg->completedLaps + 1;
}

void LapData::setLapTimeValid(SPageFileGraphic* pfg) {
    validLap = pfg->isValidLap; // this needs to be done with the UDP connection thing...
    laptime = pfg->iLastTime;
}

void LapData::addCurrentSectorTime(SPageFileGraphic* pfg) {
    switch (pfg->currentSectorIndex) 
    {
        case 1:
            sector1 = pfg->lastSectorTime;
            break;
        case 2:
            sector2 = pfg->lastSectorTime - sector1;
            break;
        case 0: // check if 3 or 0 is the correct value to look for
                // it should be 0...
            sector3 = laptime - sector1 - sector2;
            break;
    }
}

void LapData::print() {
    printf("%d\n\t%d\n\t%d\n\t%d\n\t%d\n", currentLap, laptime, sector1, sector2, sector3);
}

// maybe we need a function that creates the array of laps for the current session
// then we can return this array and send this array to the updateLap function during that session
// for a new session we can then store the array, delete it
// and create a new array if a new session starts
std::vector<LapData> createLaps() {
    std::vector<LapData> laps;
    return laps; // this should probs be done with pointers, need to test...
}

// Everytime this function gets called check if we are on a new lap or in a new sector
// and update values accordingly
// atm this functions updates values even if they are already stored, but thats okay for now
std::vector<LapData> updateLap(SPageFileGraphic* pfg, std::vector<LapData> laps) {
    // if empty we cannot compare yet to last lap
    if (laps.empty()) {
        LapData current;
        current.setLapNumber(pfg);
        laps.push_back(current);
    } else if (pfg->completedLaps >= laps.back().getLapNumber()) {
        laps.back().setLapTimeValid(pfg);
        laps.back().addCurrentSectorTime(pfg);

        LapData current;
        current.setLapNumber(pfg);
        laps.push_back(current);
    } else {
        laps.back().addCurrentSectorTime(pfg);
    }

    return laps;
}
