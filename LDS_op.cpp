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

	std::vector<Object> objectCatalogue = this->generateCatalogueLDS();
	Object detectedObj = Object::Object("Object 1", { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 });

	bool handleDetection(float lat, float lon) {
		float proba_detec = 0.5; // Probability of making a detection using LDS
		std::cout << "Object detected at Lat Lon: " << lat << ", " << lon << std::endl;

		// Create the Object with a name and a vector of position/velocity
		//Object detectedObj = Object::Object ( "Object 1", {lat, lon, 0.0, 0.0} );  // Using uniform initialisation

		float rand_probability = generate_random_number(0.0, 1.0);

		if (rand_probability < 0.0) {
			std::cout << "Error generating probability" << std::endl;
			exit(1);
			return false;
		}
		if (rand_probability >= 0.5) {
			 return  matchKnownObjects(detectedObj, objectCatalogue);
		}
		else {
			LDS_fail = true;
			return false;
		}
		
	}

	std::vector<Object> generateCatalogueLDS() {
		knownObjs.emplace_back("Object 1", std::vector<double>{1.0, 2.0, 3.0});
		knownObjs.emplace_back("Object 2", std::vector<double>{4.0, 7.0, 9.0});

		return knownObjs;
	}

	bool matchKnownObjects(Object& detectedObject, const std::vector<Object>& objectCat) {
		// std::vector<Object> knownObjs = { "Object 1", "Object 2", "Object 3" }; // Set list of known objects, in future populate with things like sat lauches etc.
		//std::vector<Object> objectCatalogue = this->generateCatalogueLDS();
		bool match = false;

		//for (const auto& obj : objectCat) { // Use reference to object to allow the object itself to be altered
		for (int i = 0; i < objectCat.size(); i++) {
			if (detectedObj.getposVel() == objectCat[i].getposVel() || detectedObj.getName() == objectCat[i].getName()) {
		
			//if (obj == detectedObject) {
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


	std::string LDSDataToString(bool bool_result) {

		//bool result_match = this->matchKnownObjects();
		std::string result = "[";

		for (size_t i = 0; i < objectCatalogue.size(); i++) {
			std::string name = objectCatalogue[i].getName();
			result += name;
			if (i > 0) result += ", ";
			result += std::to_string(bool_result);
			for (int j = 0; j < objectCatalogue[i].posVel.size(); j++) {
				result += std::to_string(objectCatalogue[i].posVel[i]);
			}
		}

		result += "]";

		return result;
	}

	std::vector<std::string> passLDSData() {
		// Need to pass trajectory data and threat type (or guess of threat type)
		std::vector<std::string> ldsData;
		bool detectionResult = handleDetection(detectedObj.getposVel()[0], detectedObj.getposVel()[1]);
		std::string lDS_Data_result = this->LDSDataToString(detectionResult);

		ldsData.push_back(lDS_Data_result);

		ldsData.push_back("Trajectory: " + std::to_string(detectedObj.getposVel()[0]) + "," + std::to_string(detectedObj.getposVel()[1]));
		ldsData.push_back(std::string("Threat Type: ") + (detectionResult == true ? "Known" : "Unknown"));

		return ldsData;
	}

private:
	float time = this->getTime();
	float detectionThreshold = 10.0;
	bool LDS_fail = false; // Hide the current status of LDS operation, never show to the operator - they will never know if the thing is broken or not

	std::vector<Object> knownObjs;
};

LDS::LDS()
{
}

LDS::~LDS()
{
}