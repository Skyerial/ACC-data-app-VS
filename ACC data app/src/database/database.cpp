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
								laptime INTEGER, \
								sector1 INTEGER, \
								sector2 INTEGER, \
								sector3 INTEGER, \
								valid INTEGER, \
								in_pit INTEGER,\
								tyre TEXT, \
								condition INTEGER, \
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
	rc = sqlite3_bind_text16(stmt, 4, session.getTrack(), wcslen(session.getTrack()) * 2, SQLITE_STATIC);
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR session bind 4: %d %s\n", rc, sqlite3_errmsg(db)); }
	rc = sqlite3_bind_text16(stmt, 5, session.getCarModel(), wcslen(session.getCarModel()) * 2, SQLITE_STATIC);
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR session bind 5: %d %s\n", rc, sqlite3_errmsg(db)); }
	rc = sqlite3_bind_int(stmt, 6, session.GetOnline());
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR session bind 6: %d %s\n", rc, sqlite3_errmsg(db)); }
	rc = sqlite3_bind_int(stmt, 7, session.GetRaceNumber());
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR session bind 7: %d %s\n", rc, sqlite3_errmsg(db)); }
	rc = sqlite3_bind_text16(stmt, 8, session.GetDriverName(), wcslen(session.GetDriverName()) * 2, SQLITE_STATIC);
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR session bind 8: %d %s\n", rc, sqlite3_errmsg(db)); }
	rc = sqlite3_bind_text16(stmt, 9, session.GetDriverSurname(), wcslen(session.GetDriverSurname()) * 2, SQLITE_STATIC);
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR session bind 9: %d %s\n", rc, sqlite3_errmsg(db)); }

	rc = sqlite3_step(stmt);
	rc = sqlite3_finalize(stmt);
}

void InsertLap()
{

}

void SelectSession()
{
	sqlite3_stmt* stmt;
	int rc;

	std::string select_session_query = "SELECT date FROM sessionData";
	rc = sqlite3_prepare_v2(db, select_session_query.c_str(), (int)select_session_query.length(), &stmt, nullptr);
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR select session: %s\n", sqlite3_errmsg(db)); }

	while (sqlite3_step(stmt) == SQLITE_ROW) {
		std::wstring datetime(static_cast<const wchar_t*>(sqlite3_column_text16(stmt, 0)));
		std::wcout << datetime << std::endl;
	}

	//rc = sqlite3_step(stmt);
	//// this could be a very nice way of doing the error; shows where in the code, error code and error message
	//if (rc != SQLITE_DONE) { fprintf(stderr, "ERROR step select session: %d %s\n", rc, sqlite3_errmsg(db)); }
	//const unsigned char* datetime = sqlite3_column_text(stmt, 2);
	//std::cout << datetime << std::endl;

	rc = sqlite3_finalize(stmt);
	if (rc != SQLITE_OK) { fprintf(stderr, "ERROR finalize select session: %s\n", sqlite3_errmsg(db)); }
}

} // anonimous namespace

void InsertSessionLap(SessionData& session, std::vector<LapData>& laps)
{
	OpenDatabase();

	sqlite3_exec(db, "DROP TABLE sessionData", nullptr, nullptr, nullptr);
	CreateTables();
	InsertSession(session);
	SelectSession();
	//InsertLaps(laps);

	CloseDatabase();
}

// TEST
void MakeDB()
{
	OpenDatabase();

	SessionData session;
	std::vector<LapData> laps;

	InsertSessionLap(session, laps);

	CloseDatabase();
}
