//////////////////////////////////////////////////////////////////////////////
// Author: Daniel Oppenhuizen
// License: None
// 
// Main file, here all the seperate threads get created, along with the
// medium for them to share information.
//////////////////////////////////////////////////////////////////////////////

#include <ShlObj.h>

#include <thread>

#include "dataUITransfer.h"
#include "dataAppUI.h"
#include "Collectors.h"
#include "dataCollector.h"
#include "socket/UdpDataCollector.h"

// include needed to get full path of documents folder
// #include <C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.19041.0\\um\\ShlObj_core.h>

// get full path of known windows document folder
// maybe a bit of an old way to do this, but it works
wchar_t mydoc_path[MAX_PATH];
HRESULT hres = SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, mydoc_path);

int main(int argc, char* argv[])
{
	if (hres != S_OK) { return 1; } // check if my document path was actually found

	ui_data_pair pair;
	pair.UI_running = true;
	pair.lapnumber = 0;
	pair.session_active = false;
	pair.in_pit = true;				// Starts true since car always starts in pit

	// since we only use two threads atm there no need to make a vector threadpool
	// this is good to do once we add more threads tho
	std::jthread ui_thread(UIRenderer, mydoc_path, std::ref(pair));
	std::jthread collector_thread(collectors, std::ref(pair));
	//std::jthread data_thread(DataCollector, std::ref(pair));
	//std::jthread udp_thread(UdpDataCollector, std::ref(pair));

	ui_thread.join();
	collector_thread.join();
	//data_thread.join();
	//udp_thread.join();

	return 0;
}