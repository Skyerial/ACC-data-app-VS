#pragma once
#include "nlohmann/json.hpp"

using json = nlohmann::json;

class SessionData
{
private:
    wchar_t* track;
    wchar_t* carModel;
    int bestLap;

public:
    wchar_t* getTrack() { return track; }
    wchar_t* getCarModel() { return carModel; }
    int getBestLap() { return bestLap; }

    void setSessionData(SPageFileStatic* pfs);
    void setBestLap();

    friend void to_json(json& j, SessionData data) {
        j["track: "] = data.track;
        j["car: "] = data.carModel;
        j["best lap: "] = data.bestLap;
    }

    void print();
};
