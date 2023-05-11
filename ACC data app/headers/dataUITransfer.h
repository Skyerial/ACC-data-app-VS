#pragma once
#include <atomic>

// This struct is used as a communication box between the UI and the collector.
// Both know this exists and can read/write these values. Since the variables
// are atomic only one thread can access the variable at a time.
using ui_data_pair = struct {
	std::atomic<bool> UI_running;
	std::atomic<int> lapnumber;
	std::atomic<bool> session_active;
};
