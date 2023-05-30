#pragma once
#include "nlohmann/json.hpp"
#include "SharedFileOut.h"
#include "Lapdata.h"

using json = nlohmann::json;

// This class is used to keep all the session related data together. The intend
// is to create an object of this class when the session starts, gather all the
// wanted data and store it. After that the object only gets saved when the
// session ends. The object data shouldn't be changed at any other point.
class SessionData
{
private:
    std::wstring date;
    AC_SESSION_TYPE session;
    int best_lap; // this has its own set function
    wchar_t* circuit;
    wchar_t* car;
    int online;
    int race_number; // seems to not be possible to get with sharedmem
    wchar_t* driver_name;
    wchar_t* driver_surname;

public:
    std::wstring GetDate() { return date;  }
    AC_SESSION_TYPE getSession() { return session; }
    int getBestLap() { return best_lap; }
    wchar_t* getTrack() { return circuit; }
    wchar_t* getCarModel() { return car; }
    int GetOnline() { return online; }
    int GetRaceNumber() { return race_number; }
    wchar_t* GetDriverName() { return driver_name; }
    wchar_t* GetDriverSurname() { return driver_surname; }

    void SetSessionData(SPageFileStatic* pfs, SPageFileGraphic* pfg);
    void SetBestLap(std::vector<LapData>& laps);

    friend void to_json(json& j, SessionData data) {
        j["session"] = data.session;
        j["track"] = data.circuit;
        j["car"] = data.car;
        j["best lap"] = data.best_lap;
    }

    void Print();
};
