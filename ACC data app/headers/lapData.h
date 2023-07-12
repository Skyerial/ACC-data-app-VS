#pragma once
#include "SharedFileOut.h"
#include <vector>

// This class is used to keep all the data of lap together. So the intend is
// to create a new object for every lap and make sure all the data of that lap is
// stored in that object.
//
// It also has functions to change the values of the lap object when new 
// information for that lap becomes available.
class LapData
{
public:
    // constructor
    LapData();
    LapData(int lapnumber, int valid, int laptime, int sector1,
            int sector2, int sector3, int in_pit, int position, int id);

    int getLapNumber() { return currentLap_; }
    int GetValidLap() { return validLap_; }
    int getLapTime() { return laptime_; }
    int getSector1() { return sector1_; }
    int getSector2() { return sector2_; }
    int getSector3() { return sector3_; }
    int GetInPit() { return in_pit_; }
    int GetPostition() { return position_; }
    int GetId() { return id_; }

    void SetLapNumber(const SPageFileGraphic* pfg);
    void SetLapTimeValid(const SPageFileGraphic* pfg);
    void AddCurrentSectorTime(const SPageFileGraphic* pfg);
    void SetInPit(const SPageFileGraphic* pfg);
    void SetPosition(const SPageFileGraphic* pfg);

    // TESTING
    void Print() const;

private:
    int currentLap_;
    int validLap_;      // not implemented yet
    int laptime_;       // in miliseconds
    int sector1_;       // ^
    int sector2_;       // ^
    int sector3_;       // ^
    int in_pit_;
    int position_;
    int id_;            // Only used when retrieving from database
};

std::vector<LapData> CreateLaps();

// UpdateLap gets the right memory location to check for data that is available.
// Once it sees that there is new information is available for the given lap
// it updates that lap.
void UpdateLap(SPageFileGraphic* pfg, std::vector<LapData>& laps);
