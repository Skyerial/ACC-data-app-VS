#pragma once
#include "nlohmann/json.hpp"

using json = nlohmann::json;

class SessionData
{
private:
    AC_SESSION_TYPE session;
    wchar_t* track;
    wchar_t* carModel;
    int bestLap;

public:
    AC_SESSION_TYPE getSession() { return session; }
    wchar_t* getTrack() { return track; }
    wchar_t* getCarModel() { return carModel; }
    int getBestLap() { return bestLap; }

    void setSessionData(SPageFileStatic* pfs, SPageFileGraphic* pfg);
    void setBestLap();

    friend void to_json(json& j, SessionData data) {
        j["session: "] = data.session;
        j["track: "] = data.track;
        j["car: "] = data.carModel;
        j["best lap: "] = data.bestLap;
    }

    void print();
};
