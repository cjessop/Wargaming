#include <fstream>
//#include <stdexcep>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <cmath>
#include <vector>
#include <math.h>
#include <random>
#include "Utils.h"
#include "Object.h"

/* Read the total time of the scenario in real seconds - eventually replace with what will 
provided by Python side of the controller */
float readTime(const std::string& trajFile) {
	std::ifstream InputFile(trajFile);

	try {
		if (!InputFile.is_open()) {
			std::cerr << "Error reading the trajectory file" << std::endl;
			throw std::invalid_argument("Recieved an incorrect input file");
		}
	}
	catch( ... ) {
		std::cout << "Exception occured" << std::endl;
	};

	std::string line;

	//std::cout << "Total time of flight: " << std::endl;

	while (std::getline(InputFile, line)) {
		//std::cout << line << std::endl;
	}

	float tote_time = std::stof(line);

	return tote_time;
}

std::string readRadar(const std::string& radFile) {
	// Declare member variables for the readRadar method
	std::ifstream InputFile(radFile);
	std::vector<std::string> objectNames;
	std::string read_line, temp;
	std::string RadarType;
	std::vector<std::string> wordVec;

	try {
		if (!InputFile.is_open()) {
			std::cerr << "Error opening the radar definition file" << std::endl;
			throw std::invalid_argument("Recieved an incorrect input file");
		}
	}
	catch( ... ) {
		std::cout << "Exception occured" << std::endl;
	};

	while (std::getline(InputFile, read_line)) {
		int switch_val;

		if (read_line.find("EWR") == std::string::npos) {
			switch_val = 1;
		}
		else if (read_line.find("FCR") == std::string::npos) {
			switch_val = 2;
		}
		else if (read_line.find("ABM") == std::string::npos) {
			switch_val = 3;
		}
		else {
			std::cout << "No radar or ABM site detected in the radar file " << std::endl;
			switch_val = 0;
		}

		// Switch statement no longer required but I want to keep it in
		switch (switch_val)
		{
		case (0):
			std::cout << "No radar detected " << std::endl;
			break;
		case (1):
			std::cout << "EWR radar detected at: " << std::endl;
			break;
		case (2):
			std::cout << "FCR radar detected at: " << std::endl;
			break;
		case (3):
			std::cout << "ABM site detected at: " << std::endl;
			break;
		default:
			break;
		}
	}


}

float countdown(float& seconds) {

	if (seconds > 0) {
		//std::cout << "\033[2J\033[1;1H";
		std::cout << "time remaining: " << seconds << std::endl;

		std::this_thread::sleep_for(std::chrono::seconds(1));
		--seconds;
	}
	else {
		std::cout << "time has elapsed" << std::endl;
	}
	
	return seconds;
}


float check_progress(float current_time) {

	float total_time = readTime("Trajec.txt");
	float frac_progress = (1.0 - (current_time / total_time)) * float(100);

	std::cout << "\033[2J\033[1;1H";
	std::cout << "Total launch Progress: " << std::round(frac_progress) << "%" << std::endl;


	return frac_progress;
}


std::vector<float> get_ll(std::string lla_file) {
	std::ifstream InputFile(lla_file);

	if (!InputFile.is_open()) {
		std::cerr << "Error opening ll information file" << std::endl;
		exit;
	}

	std::string line;
	std::vector<float> ll_list;

	while (std::getline(InputFile, line)) {
		//std::cout << line << std::endl;
		float ll = std::stof(line);
		ll_list.push_back(ll);

		std::cout << ll_list[0] << ll_list[1] << std::endl;
	}

	return ll_list;
}

int lineNum(std::string InputFile) {
	int number_of_lines = 0;
	std::string line;
	std::ifstream myfile(InputFile);

	while (std::getline(myfile, line))
		++number_of_lines;
	std::cout << "Number of lines in text file: " << number_of_lines;
	return number_of_lines;
}

std::size_t count_lines(const std::string& filename) {
	std::ifstream ifs(filename);
	if (!ifs) {
		throw std::runtime_error("Failed open file ");
	}

	std::string line;
	size_t counter = 0;
	while (std::getline(ifs, line)) counter++;
	return counter;
}

/* Read objects from the object catalogue text file to populate a vector of objects */
std::vector<std::string> readCatalogue(const std::string& catalogueFile) {
	std::ifstream InputFile(catalogueFile);
	if (!InputFile.is_open()) {
		std::cerr << "Error opening the catalogue file" << std::endl;
		exit(1);  // Use exit(1) instead of just exit
	}
	std::string line;
	std::vector<std::string> objectList;
	while (std::getline(InputFile, line)) {
		objectList.push_back(line);
	}
	return objectList;
}

std::string ObjectNameGet(std::string& object) {
	std::string name = object;
	std::string delimiter = ",";
	std::string token = name.substr(0, name.find(delimiter));

	return token;
}

std::vector<std::string> ObjectDetailsGet(std::string& object) {
	std::vector<std::string> s_vec;
	std::string s = object;
	std::string delimiter = ",";
	size_t pos = 0;
	std::string token;

	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		s_vec.push_back(token);
		s.erase(0, pos + delimiter.length());

	}

	return s_vec;
}

Object buildObject(std::vector<std::string>& objectList) {
	std::string name = objectList[0];
	double val1 = std::stof(objectList[1]);
	double val2 = std::stof(objectList[2]);
	double val3 = std::stof(objectList[3]);

	Object object(name, std::vector<double>{ val1, val2, val3 });

	return object;
}



double generate_random_number(float num1, float num2) {
	std::random_device rd;

	std::mt19937 gen(rd());

	std::uniform_real_distribution<> dis(num1, num2);

	return dis(gen);
}

/* Generate a random integer within the range of num1 and num2 */
int generate_random_int(int num1, int num2) {
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<std::mt19937::result_type> dist(num1, num2);
	
	//std::cout << dist(rng);
	return dist(rng);
}