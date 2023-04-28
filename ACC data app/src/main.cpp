#include <thread>

#include "dataUITransfer.h"
#include "dataAppUI.h"
#include "dataCollector.h"

// include needed to get full path of documents folder
// #include <C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.19041.0\\um\\ShlObj_core.h>
#include <ShlObj.h>

// get full path of known windows document folder
// maybe a bit of an old way to do this, but it works
wchar_t mydoc_path[MAX_PATH];
HRESULT hres = SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, mydoc_path);

int main(int argc, char* argv[])
{
	ui_data_pair pair;
	pair.UI_running = true;
	pair.lapnumber = 0;
	pair.session_active = false;

	// since we only use two threads atm there no need to make a vector threadpool
	// this is good to do once we add more threads tho
	std::jthread ui_thread(UIRenderer, mydoc_path, std::ref(pair));
	std::jthread data_thread(dataCollector, mydoc_path, std::ref(pair));

	ui_thread.join();
	data_thread.join();
	return 0;
}