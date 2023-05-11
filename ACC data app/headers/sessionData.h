#pragma once
#include "nlohmann/json.hpp"

using json = nlohmann::json;

// This class is used to keep all the session related data together. The intend
// is to create an object of this class when the session starts, gather all the
// wanted data and store it. After that the object only gets saved when the
// session ends. The object data shouldn't be changed at any other point.
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

    void SetSessionData(SPageFileStatic* pfs, SPageFileGraphic* pfg);
    void SetBestLap();

    friend void to_json(json& j, SessionData data) {
        j["session"] = data.session;
        j["track"] = data.track;
        j["car"] = data.carModel;
        j["best lap"] = data.bestLap;
    }

    void Print();
};
