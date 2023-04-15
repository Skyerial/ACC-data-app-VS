#include "stdafx.h"
#include "dataToFile.h"
#include "nlohmann\json.hpp"
#include "lapData.h"

#include <fstream>

// test
#include <iostream>

using json = nlohmann::json;

std::ofstream outfile;

// this may be kind of dumb openFile en closeFile
// figure out if opening a stream can also be used by other files
// meaning that if I make ofstream outfile in two files that both can read and
// write to the same file?... 
void openFile(std::wstring file_location) {
	std::cout << "we arrived" << std::endl;
	outfile.open(file_location);
	if (outfile.is_open()) {
		std::cout << "opened the file\n" << std::endl;
	}
}

void closeFile() {
	outfile.close();
}

void writeToFile(json& j) {
	outfile << j << "\n";
}












// not sure if these are needed yet....
void printToFile(std::string name, float value) {
	outfile << name.c_str() << " : " << value << std::endl;
}

template <typename T, unsigned S>
inline void printToFile(const std::string name, const T(&v)[S]) {
	// this is not needed since we want a data file with values without names
	// order of values is documented
	// outfile << name.c_str() << " : ";

	for (int i = 0; i < S; i++)
	{
		outfile << v[i];
		if (i < S - 1)
		{
			outfile << ", ";
		}

	}
	outfile << std::endl;
}

// This gives values from the asked data as a string to put in the json file
template <typename T, unsigned S>
inline std::string actualValue(const T(&v)[S]) {
	std::string result;
	for (int i = 0; i < S; i++) {
		float temp = v[i];
		result = result + std::to_string(temp);
		if (i < S - 1) {
			result = result + " , ";
		}
	}
	return result;
}
