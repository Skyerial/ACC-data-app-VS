#include "pch.h"

#include <vector>

#include "sessionData.h"
#include "lapData.h"
#include "database/database.h"

TEST(DatabaseTest, DatabaseInsertSessionLap)
{
	SessionData session;
	LapData lap1(1, 0, 0, 0, 0, 0, 0, 0);
	LapData lap2(2,0, 0 ,0 ,0 ,0 ,0 ,0);
	std::vector<LapData> laps;
	laps.push_back(lap1);
	laps.push_back(lap2);
	ASSERT_EQ(0, InsertSessionLap(session, laps));
}

TEST(DatabaseTest, DatabaseRetrieveSession)
{
	std::vector<SessionData> session_test;
	RetrieveSession(session_test, 1, 0);
	ASSERT_EQ(1, session_test.size());
	ASSERT_EQ(1, session_test[0].GetId());
	ASSERT_EQ(-1, session_test[0].getSession());
}

TEST(DatabaseTest, DatabaseRetrieveLaps)
{
	std::vector<LapData> laps_test;
	std::vector<SessionData> session_test;
	RetrieveSession(session_test, 1, 0);
	RetrieveLaps(laps_test, session_test[0].GetId());
	ASSERT_EQ(1, laps_test[0].getLapNumber());
	ASSERT_EQ(2, laps_test[1].getLapNumber());
}


// lil test to check if the google unit testing itself works at least
//TEST(Test0, all)
//{
//	EXPECT_EQ(true, true);
//}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
