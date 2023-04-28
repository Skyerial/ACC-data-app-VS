#pragma once
#include <atomic>

using ui_data_pair = struct {
	std::atomic<bool> UI_running;
	std::atomic<int> lapnumber;
	std::atomic<bool> session_active;
};
