#include <iostream>
#include <math.h>
#include <vector>
#include <stdio.h>
#include "Utils.h"
#include <random>
#include "Object.h"

class LDS
	/* This is the class that represents all of the methods and attributes that the LDS operator has access to and 
	actions they can perform */
{
public:
	LDS(); // Constructor
	~LDS(); // Destructor

	void setTime(float t) {
		float time = t;
	}

	float getTime() { // Getter function for the current time
		return time;
	}

	void verifyTime() {
		float currentTime = this->getTime();
	}

	// Create function to evaluate the progress of the 
	void evaluate_progress(float current_time) { 
		float progress = check_progress(current_time);
		if (progress <= detectionThreshold) {
			std::vector<float> ll = get_ll("lla.txt");
			handleDetection(ll[0], ll[1]);
		}
	}

	void handleDetection(float lat, float lon) {
		float proba_detec = 0.5; // Probability of making a detection using LDS
		std::cout << "Object detected at Lat Lon: " << lat << lon << std::endl;

		Object detectedObj = "Object 1", std::vector<double>{1.0, 2.0, 3.0};

		float rand_probabilty = generate_random_number(0.0, 1.0);

		if (!rand_probabilty) {
			std::cout << "Error generating probability" << std::endl;
			exit;
		}

		if (rand_probabilty >= 0.5) {
			matchKnownObjects();
		}
		else {
			LDS_fail = true;
		}

	}

	std::vector<Object> generateCatalogueLDS() {
		knownObjs.emplace_back("Object 1", std::vector<double>{1.0, 2.0, 3.0})
		knownObjs.emplace_back("Object 2", std::vector<double>{4.0, 7.0, 9.0});

		return knownObjs;
	}

	bool matchKnownObjects() {
		// std::vector<Object> knownObjs = { "Object 1", "Object 2", "Object 3" }; // Set list of known objects, in future populate with things like sat lauches etc.

		bool match = false;

		for (const auto& obj : knownObjs) { // Use reference to object to allow the object itself to be altered
			if (obj == detectedObj) {
				std::cout << "Detected signature matches known signature and is safe" << std::endl;
				match = true;
				return match;
				// Add additional logic here to deal with the specifics and next steps if objects match
			}
			if (!match) {
				std::cout << "Detected signature does not match that in existing database" << std::endl;
				std::cout << "Assume detected signature is hostile in nature" << std::endl;

				match = false;
				return match;
			}
		}
	}


	std::string LDSDataToString(const std::vector<Object>& KnownObjects) {

		bool result_match = this->matchKnownObjects();
		std::string result = "[";

		for (size_t i = 0; i < KnownObjects.size(); i++) {
			std::string name = KnownObjects.getName();
			result += name;
			if (i > 0) result += ", ";
			result += std::to_string(result_match);
			result += std::to_string(posVel[i]);
		}

		result += "]";

		return result;
	}

	std::vector<std::string> passLDSData() {
		// Need to pass trajectory data and threat type (or guess of threat type)
		std::vector<std::string> ldsData = this->LDSDataToString(knownObjs);
	}

private:
	float time = this->getTime();
	float detectionThreshold = 10.0;
	bool LDS_fail = false; // Hide the current status of LDS operation, never show to the operator - they will never know if the thing is broken or not

	std::vector<Object> knownObjs
};

LDS::LDS()
{
}

LDS::~LDS()
{
}