#pragma once
#include <string>

// Function is responsible for rendering the UI.
// Input:
//	mydoc_path = string that contains path to the my documents folder
//	pair       = struct that is given to the collector and UI to communicate
//				the state of the game so that the live window can be updated 
//				accordingly.
void UIRenderer(std::wstring mydoc_path, ui_data_pair& pair);
