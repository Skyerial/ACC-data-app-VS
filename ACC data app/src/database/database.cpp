//////////////////////////////////////////////////////////////////////////////
// Author: Daniel Oppenhuizen
// License: None
// 
// This file handles all the interactions with the database and has a couple
// of functions to access this functionality
//////////////////////////////////////////////////////////////////////////////

#include "database/database.h"

#include <iostream>
#include <string>
#include <filesystem>

#include "database/sqlite3.h"
#include "sessionData.h"
#include "lapData.h"

#pragma warning(disable: 4996) // this disables getenv warning since
							   // it doesnt apply for this case
const char* appdata_path = std::getenv("APPDATA");

sqlite3* db;

namespace
{
/// <summary>
/// SOME HELPER FUNCTIONS THAT SHOULD GET THERE OWN PLACE ONCE THIS SHIT WORKS
/// </summary>
// for now GetDateTime() is placed inside sessionData.cpp
std::wstring GetDateTime()
{
	time_t rawtime;
	struct tm timeinfo;
	wchar_t buffer[20];

	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);

	wcsftime(buffer, 20, L"%Y-%m-%d %H:%M:%S", &timeinfo);

	std::wcout << buffer << std::endl;

	return buffer;
}

int CorrespondingSessionID(SessionData& session)
{
	sqlite3_stmt* stmt;
	int rc;
	std::string session_id_query = "SELECT id FROM sessionData WHERE date = ?";
	rc = sqlite3_prepare_v2(db, session_id_query.c_str(), (int)session_id_query.length(), &stmt, nullptr);
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR sessionID: %d %s\n", rc, sqlite3_errmsg(db)); return -1; }

	rc = sqlite3_bind_text16(stmt, 1, session.GetDate().c_str(), (int)session.GetDate().length() * 2, SQLITE_STATIC);
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR sessionID bind: %d %s\n", rc, sqlite3_errmsg(db)); return -2; }

	int session_id = -5;
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		session_id = sqlite3_column_int(stmt, 0);
	}
	//if (rc != SQLITE_DONE) { fprintf(stderr, "ERROR sessionID step: %d %s\n", rc, sqlite3_errmsg(db)); return -3; }

	rc = sqlite3_finalize(stmt);
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR sessionID finalize: %d %s\n", rc, sqlite3_errmsg(db)); return -4; }

	return session_id;
}
/// <summary>
/// END OF THESE HELPER FUNCTIONS
/// </summary>

int OpenDatabase()
{
	std::string appdata_cpy;
	appdata_cpy.append(appdata_path);
	appdata_cpy.append("\\ACC data app");
	
	if (!std::filesystem::exists(appdata_cpy))
		std::filesystem::create_directory(appdata_cpy);
	
	appdata_cpy.append("\\sessions.db");

	if (sqlite3_open(appdata_cpy.c_str(), &db))
	{
		fprintf(stderr, "ERROR open: %s\n", sqlite3_errmsg(db));
		return(1);
	}
	sqlite3_busy_timeout(db, 1000);

	return(0);
}

int CloseDatabase()
{
	int rc = sqlite3_close(db);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "ERROR close: %d %s\n", rc, sqlite3_errmsg(db));
		return(1);
	}

	return 0;
}

int CreateTables()
{
	const char* sessiondata_table = "CREATE TABLE IF NOT EXISTS sessionData \
									(id INTEGER PRIMARY KEY, \
									date DATETIME, \
									session INTEGER, \
									best_lap INTEGER, \
									circuit TEXT, \
									car TEXT, \
									online INTEGER, \
									race_number INTEGER, \
									driver_name TEXT, \
									driver_surname TEXT)";
	if (sqlite3_exec(db, sessiondata_table, nullptr, nullptr, nullptr) != SQLITE_OK)
	{
		fprintf(stderr, "ERROR session table: %s\n", sqlite3_errmsg(db));
		return(1);
	}
	
	// not sure if primary key is needed if we dont use it for lapnumber or anything
	const char* lapdata_table = "CREATE TABLE IF NOT EXISTS lapData \
								(id INTEGER PRIMARY KEY, \
								lapnumber INTEGER, \
								valid INTEGER, \
								laptime INTEGER, \
								sector1 INTEGER, \
								sector2 INTEGER, \
								sector3 INTEGER, \
								in_pit INTEGER, \
								position INTEGER, \
								session_id INTEGER, \
								FOREIGN KEY(session_id) REFERENCES sessionData(id))";
	if (sqlite3_exec(db, lapdata_table, nullptr, nullptr, nullptr) != SQLITE_OK)
	{
		fprintf(stderr, "ERROR lap table: %s\n", sqlite3_errmsg(db));
		return(1);
	}

	return 0;
}

void InsertSession(SessionData& session)
{
	sqlite3_stmt* stmt;
	int rc;
	std::string insert_session_query = "INSERT INTO sessionData \
										(DATE, SESSION, BEST_LAP, CIRCUIT, \
										CAR, ONLINE, RACE_NUMBER, \
										DRIVER_NAME, DRIVER_SURNAME) \
										VALUES \
										(?,?,?,?,?,?,?,?,?)";
	if (sqlite3_prepare_v2(db, insert_session_query.c_str(),
		(int)insert_session_query.length(), &stmt, nullptr) != SQLITE_OK)
	{
		fprintf(stderr, "ERROR insert session: %s\n", sqlite3_errmsg(db));
	}

	std::wstring date_time = session.GetDate();
	// this starts at one since 0 is the primary key and gets done automatically
	rc = sqlite3_bind_text16(stmt, 1, date_time.c_str(), (int)date_time.length() * 2, SQLITE_STATIC);
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR session bind 1: %d %s\n", rc, sqlite3_errmsg(db)); }
	rc = sqlite3_bind_int(stmt, 2, session.getSession());
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR session bind 2: %d %s\n", rc, sqlite3_errmsg(db)); }
	rc = sqlite3_bind_int(stmt, 3, session.getBestLap());
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR session bind 3: %d %s\n", rc, sqlite3_errmsg(db)); }
	rc = sqlite3_bind_text16(stmt, 4, session.getTrack().c_str(), session.getTrack().size() * 2, SQLITE_STATIC);
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR session bind 4: %d %s\n", rc, sqlite3_errmsg(db)); }
	rc = sqlite3_bind_text16(stmt, 5, session.getCarModel().c_str(), session.getCarModel().size() * 2, SQLITE_STATIC);
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR session bind 5: %d %s\n", rc, sqlite3_errmsg(db)); }
	rc = sqlite3_bind_int(stmt, 6, session.GetOnline());
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR session bind 6: %d %s\n", rc, sqlite3_errmsg(db)); }
	rc = sqlite3_bind_int(stmt, 7, session.GetRaceNumber());
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR session bind 7: %d %s\n", rc, sqlite3_errmsg(db)); }
	rc = sqlite3_bind_text16(stmt, 8, session.GetDriverName().c_str(), session.GetDriverName().size() * 2, SQLITE_STATIC);
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR session bind 8: %d %s\n", rc, sqlite3_errmsg(db)); }
	rc = sqlite3_bind_text16(stmt, 9, session.GetDriverSurname().c_str(), session.GetDriverSurname().size() * 2, SQLITE_STATIC);
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR session bind 9: %d %s\n", rc, sqlite3_errmsg(db)); }

	rc = sqlite3_step(stmt);
	rc = sqlite3_finalize(stmt);
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR finalize session: %d %s\n", rc, sqlite3_errmsg(db)); }
}

void InsertLaps(std::vector<LapData>& laps, int session_id)
{
	sqlite3_stmt* stmt;
	int rc;
	std::string insert_lap_query = "INSERT INTO lapData \
									(LAPNUMBER, VALID, LAPTIME, SECTOR1, \
									SECTOR2, SECTOR3, IN_PIT, POSITION, SESSION_ID) \
									VALUES \
									(?,?,?,?,?,?,?,?,?)";
	rc = sqlite3_prepare_v2(db, insert_lap_query.c_str(), (int)insert_lap_query.length(), &stmt, nullptr);
	if (rc != SQLITE_OK)
		fprintf(stderr, "ERROR insert laps: %d %s\n", rc, sqlite3_errmsg(db));

	for (size_t i = 0; i < laps.size(); i++)
	{
		// binds
		rc = sqlite3_reset(stmt);
		if (rc != SQLITE_OK) { fprintf(stderr, "ERROR reset laps: %d %s\n", rc, sqlite3_errmsg(db)); }

//int lapNumber = laps[i].getLapNumber();
//int valid = laps[i].GetValidLap();
//int getLapTime = laps[i].getLapTime();
//
//insert_lap_query (..... lapNumber, valid, getLapTime)
////laps[i]

		rc = sqlite3_bind_int(stmt, 1, laps[i].getLapNumber());
		if (rc != SQLITE_OK) { fprintf(stderr, "ERROR laps bind 1: %d %s\n", rc, sqlite3_errmsg(db)); }
		rc = sqlite3_bind_int(stmt, 2, laps[i].GetValidLap());
		if (rc != SQLITE_OK) { fprintf(stderr, "ERROR laps bind 2: %d %s\n", rc, sqlite3_errmsg(db)); }
		rc = sqlite3_bind_int(stmt, 3, laps[i].getLapTime());
		if (rc != SQLITE_OK) { fprintf(stderr, "ERROR laps bind 3: %d %s\n", rc, sqlite3_errmsg(db)); }
		rc = sqlite3_bind_int(stmt, 4, laps[i].getSector1());
		if (rc != SQLITE_OK) { fprintf(stderr, "ERROR laps bind 4: %d %s\n", rc, sqlite3_errmsg(db)); }
		rc = sqlite3_bind_int(stmt, 5, laps[i].getSector2());
		if (rc != SQLITE_OK) { fprintf(stderr, "ERROR laps bind 5: %d %s\n", rc, sqlite3_errmsg(db)); }
		rc = sqlite3_bind_int(stmt, 6, laps[i].getSector3());
		if (rc != SQLITE_OK) { fprintf(stderr, "ERROR laps bind 6: %d %s\n", rc, sqlite3_errmsg(db)); }
		rc = sqlite3_bind_int(stmt, 7, laps[i].GetInPit());
		if (rc != SQLITE_OK) { fprintf(stderr, "ERROR laps bind 7: %d %s\n", rc, sqlite3_errmsg(db)); }
		rc = sqlite3_bind_int(stmt, 8, laps[i].GetPostition());
		if (rc != SQLITE_OK) { fprintf(stderr, "ERROR laps bind 8: %d %s\n", rc, sqlite3_errmsg(db)); }
		rc = sqlite3_bind_int(stmt, 9, session_id);
		if (rc != SQLITE_OK) { fprintf(stderr, "ERROR laps bind 9: %d %s\n", rc, sqlite3_errmsg(db)); }

		// step
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_DONE) { fprintf(stderr, "ERROR step: %d %s\n", rc, sqlite3_errmsg(db)); }
	}

	rc = sqlite3_finalize(stmt);
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR finalize laps: %d %s\n", rc, sqlite3_errmsg(db)); }
}

void SelectSession(std::vector<SessionData>& session, int limit, int offset)
{
	sqlite3_stmt* stmt;
	int rc;

	// The join statement takes a session and makes a row with the data of that session and one of the laps
	// that has the corresponding ID. Every lapdata row gets its own row that also has the sessionData row
	std::string select_session_query = "SELECT * FROM sessionData \
										ORDER BY id \
										LIMIT ? OFFSET ?";
	rc = sqlite3_prepare_v2(db, select_session_query.c_str(), (int)select_session_query.length(), &stmt, nullptr);
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR select session: %s\n", sqlite3_errmsg(db)); }

	rc = sqlite3_bind_int(stmt, 1, limit);
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR select session bind 1: %d %s\n", rc, sqlite3_errmsg(db)); }
	rc = sqlite3_bind_int(stmt, 2, offset);
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR select session bind 2: %d %s\n", rc, sqlite3_errmsg(db)); }

	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
	{
		int id = sqlite3_column_int(stmt, 0);
		std::wstring datetime(static_cast<const wchar_t*>(sqlite3_column_text16(stmt, 1)));
		AC_SESSION_TYPE session_type = sqlite3_column_int(stmt, 2);
		int best_lap = sqlite3_column_int(stmt, 3);
		std::wstring circuit(static_cast<const wchar_t*>(sqlite3_column_text16(stmt, 4)));
		std::wstring car(static_cast<const wchar_t*>(sqlite3_column_text16(stmt, 5)));
		int online = sqlite3_column_int(stmt, 6);
		int race_number = sqlite3_column_int(stmt, 7);
		std::wstring driver_name(static_cast<const wchar_t*>(sqlite3_column_text16(stmt, 8)));
		std::wstring driver_surname(static_cast<const wchar_t*>(sqlite3_column_text16(stmt, 9)));

		SessionData current_session(id, datetime, session_type, best_lap, circuit, car, online,
									race_number, driver_name, driver_surname);

		session.push_back(current_session);
		//// column selections for the sessionData
		//int session_id = sqlite3_column_int(stmt, 0);
		//std::wstring datetime(static_cast<const wchar_t*>(sqlite3_column_text16(stmt, 1)));

		//// column selections for the lapData
		//int lap_id = sqlite3_column_int(stmt, 10);
		//int lap_session_id = sqlite3_column_int(stmt, 19);
		//std::wcout << session_id << "\t" << datetime << "\t" << lap_id << "\t" << lap_session_id << std::endl;
	}
	if (rc != SQLITE_DONE) { fprintf(stderr, "ERROR step select session: %d %s\n", rc, sqlite3_errmsg(db)); }

	rc = sqlite3_finalize(stmt);
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR finalize select session: %s\n", sqlite3_errmsg(db)); }
}

void SelectLaps(std::vector<LapData>& laps, int session_id)
{
	sqlite3_stmt* stmt;
	int rc;

	const std::string select_laps_query = "SELECT * FROM lapData WHERE session_id = ? ORDER BY id";
	rc = sqlite3_prepare_v2(db, select_laps_query.c_str(), static_cast<int>(select_laps_query.length()), &stmt, nullptr);
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR select laps: %s\n", sqlite3_errmsg(db)); }

	rc = sqlite3_bind_int(stmt, 1, session_id);
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR select laps bind: %d %s\n", rc, sqlite3_errmsg(db)); }

	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
	{
		int lapnumber = sqlite3_column_int(stmt, 1);
		int valid = sqlite3_column_int(stmt, 2);
		int laptime = sqlite3_column_int(stmt, 3);
		int sector1 = sqlite3_column_int(stmt, 4);
		int sector2 = sqlite3_column_int(stmt, 5);
		int sector3 = sqlite3_column_int(stmt, 6);
		int in_pit = sqlite3_column_int(stmt, 7);
		int position = sqlite3_column_int(stmt, 8);
		int id = sqlite3_column_int(stmt, 9);

		LapData current_lap(lapnumber, valid, laptime, sector1, sector2, sector3,
							in_pit, position, id);
		laps.push_back(current_lap);
	}
	if (rc != SQLITE_DONE) { fprintf(stderr, "ERROR step select laps: %d %s\n", rc, sqlite3_errmsg(db)); }

	rc = sqlite3_finalize(stmt);
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR finalize select laps: %s\n", sqlite3_errmsg(db)); }
}

void DeleteSession(const int session_id)
{
	sqlite3_stmt* stmt;
	int rc;

	const std::string delete_query = "DELETE FROM sessionData WHERE id = ?";
	rc = sqlite3_prepare_v2(db, delete_query.c_str(), static_cast<int>(delete_query.length()), &stmt, nullptr);
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR delete session: %s\n", sqlite3_errmsg(db)); }

	rc = sqlite3_bind_int(stmt, 1, session_id);
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR delete session bind: %d %s\n", rc, sqlite3_errmsg(db)); }

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) { fprintf(stderr, "ERROR delete session step: %d %s\n", rc, sqlite3_errmsg(db)); }

	rc = sqlite3_finalize(stmt);
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR delete session finalize: %s\n", sqlite3_errmsg(db)); }
}

void DeleteLaps(const int session_id)
{
	sqlite3_stmt* stmt;
	int rc;

	const std::string delete_query = "DELETE FROM lapData WHERE session_id = ?";
	rc = sqlite3_prepare_v2(db, delete_query.c_str(), static_cast<int>(delete_query.length()), &stmt, nullptr);
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR delete laps: %s\n", sqlite3_errmsg(db)); }

	rc = sqlite3_bind_int(stmt, 1, session_id);
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR delete laps bind: %d %s\n", rc, sqlite3_errmsg(db)); }

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) { fprintf(stderr, "ERROR delete laps step: %d %s\n", rc, sqlite3_errmsg(db)); }

	rc = sqlite3_finalize(stmt);
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR delete laps finalize: %s\n", sqlite3_errmsg(db)); }
}

} // anonymous namespace

int InsertSessionLap(SessionData& session, std::vector<LapData>& laps)
{
	OpenDatabase();
	InsertSession(session);
	const int session_id = CorrespondingSessionID(session); // this has to wait until the previous function is done
													  // otherwise this results in two sources wanting to use
													  // the database at the same time
	//int session_id = 0;
	InsertLaps(laps, session_id);
	return CloseDatabase();
}

// Returns a SessionData vector, should be taken as const reference and not copied
// Takes the amount of session to retrieve and from which ID onward
void RetrieveSession(std::vector<SessionData>& session, const int limit, const int offset)
{
	OpenDatabase();
	SelectSession(session, limit, offset);
	CloseDatabase();
}

void RetrieveLaps(std::vector<LapData>& laps, const int session_id)
{
	OpenDatabase();
	SelectLaps(laps, session_id);
	CloseDatabase();
}

void DeleteSessionLaps(const int session_id)
{
	OpenDatabase();
	DeleteSession(session_id);
	DeleteLaps(session_id);
	CloseDatabase();
}

//////////////////////
// TESTING
//////////////////////
void ResetTable()
{
	sqlite3_exec(db, "DROP TABLE sessionData", nullptr, nullptr, nullptr);
	sqlite3_exec(db, "DROP TABLE lapData", nullptr, nullptr, nullptr);
	CreateTables();
}
