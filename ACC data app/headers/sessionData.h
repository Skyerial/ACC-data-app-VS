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
public:
    // empty constructor
    SessionData();

    // use this constructor when we set sessiondata from a database row
    SessionData(int id, std::wstring date, AC_SESSION_TYPE session, int best_lap,
        std::wstring circuit, std::wstring car, int online, int race_number,
        std::wstring driver_name, std::wstring driver_surname);
    
    // Getters
    int GetId() { return id_; }
    std::wstring GetDate() { return date_;  }
    AC_SESSION_TYPE getSession() { return session_; }
    int getBestLap() { return best_lap_; }
    std::wstring getTrack() { return circuit_; }
    std::wstring getCarModel() { return car_; }
    int GetOnline() { return online_; }
    int GetRaceNumber() { return race_number_; }
    std::wstring GetDriverName() { return driver_name_; }
    std::wstring GetDriverSurname() { return driver_surname_; }

    // logic
    void Update(SPageFileStatic* pfs, SPageFileGraphic* pfg);
    void BestLap(int laptime);

    void SetBestLap(std::vector<LapData>& laps);

    /*friend void to_json(json& j, SessionData data) {
        j["session"] = data.session;
        j["track"] = data.circuit;
        j["car"] = data.car;
        j["best lap"] = data.best_lap;
    }*/

private:
    int id_; // only used for when sessiondata is retrieved from database
    std::wstring date_;
    AC_SESSION_TYPE session_;
    int best_lap_; // this has its own set function
    std::wstring circuit_;
    std::wstring car_;
    int online_;
    int race_number_; // seems to not be possible to get with sharedmem
    std::wstring driver_name_;
    std::wstring driver_surname_;
};
