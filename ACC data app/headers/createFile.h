#pragma once
#include <string>

/* Create new file with right naming.
	Ex: practise20230413104552.json
	sessiontype, year, month, day, hour, minute, second, filetype
Input:
	session = string with current session
	file_type = string with file extension
	mydoc_path = string with users document path
Side effect:
	Opens file for current session with unique name based on
	session, date and time
*/
void newFile(int session, const std::wstring file_type, std::wstring mydoc_path);
