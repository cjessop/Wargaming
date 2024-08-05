#include <iostream>
#include <math.h>
#include <vector>
#include <stdio.h>
#include <random>
#include "Utils.h"
#include "Object.h"
#include "LDS_op.h"

LDS::LDS() {

}
LDS::~LDS() {

}
/* This is the class that represents all of the methods and attributes that the LDS operator has access to and 
actions they can perform */

void LDS::setTime(float t) {
	float time = t;
}

float LDS::getTime() { // Getter function for the current time
	return time;
}

void LDS::verifyTime() {
	float currentTime = this->getTime();
}

// This is also deprecated and the catalogue should be created in either this file or
// in the controller object 
std::vector<Object> LDS::createCatalogue() {
	std::vector<Object> objectCatalogue = this->generateCatalogueLDS();
	return objectCatalogue;
}

/* Create a catalogue from an input catalogue file. First create a vector of strings for each of the objects
within the */

// This is now deprecated in favour of createCatalogueFromFile from Utils.h
std::vector<Object> LDS::createCatalogueFromFile(const std::string& catalogueFile) {
	// read in the catalogue file
	std::vector<std::string> s_object_vector = readCatalogue(catalogueFile); 
	std::vector<Object> catalogue;

	for (int i = 0; i < s_object_vector.size(); i++) {
		for (int j = 0; j < s_object_vector[i].size(); j++) {
			// This probably will not work BUT I CAN'T TEST IT
			try {
				double d_object_value = std::stod(s_object_vector[i]);
			}
			catch( ... ) {
				std::cout << "Cannot convert string to double" << std::endl;
			};
		}
	}
}

// Create function to evaluate the progress of the 
void LDS::evaluate_progress(float current_time) { 
	float progress = check_progress(current_time);
	if (progress <= detectionThreshold) {
		std::vector<float> ll = get_ll("lla.txt");
		//handleDetection(ll[0], ll[1], );
	}
}

bool LDS::fail_noFail() {
	float rand_probability = generate_random_number(0.0, 1.0); // Initially use a randomly generated value as a placeholder
	bool b_rand_probability;

	if (rand_probability <= 0.5) {
		return b_rand_probability = false;
	}

	else if (rand_probability > 0.5) {
		return b_rand_probability = true;
	}
}

bool LDS::handleDetection(float lat, float lon, Object& detectedObject) {
	float proba_detec = 0.5; // Probability of making a detection using LDS
	std::cout << "Object detected at Lat Lon: " << lat << ", " << lon << std::endl;
	bool m_lds_fail = fail_noFail();

	// Create the Object with a name and a vector of position/velocity
	//Object detectedObj = Object::Object ( "Object 1", {lat, lon, 0.0, 0.0} );  // Using uniform initialisation

	if (!m_lds_fail) {
		// In the case that random probability was not successfully generated exit the programme
		std::cout << "Error generating probability" << std::endl;
		exit(1); // Instead of exiting handle this better?
		return false;
	}
	else if (m_lds_fail == false) {
		// If the lds_failure is false, i.e. the lds system did not fail perform target matching
		return  matchKnownObjects(detectedObj, objectCatalogue);
	}
	else if (m_lds_fail == true) {
		// If the lds_failure is returned as true, pause for an indefinite amount of time before outputting a fail message
		std::cout << "LDS failure detected" << std::endl;
	}
	
}

std::vector<Object> LDS::generateCatalogueLDS() {
	knownObjs.emplace_back("Object 1", std::vector<double>{1.0, 2.0, 3.0});
	knownObjs.emplace_back("Object 2", std::vector<double>{4.0, 7.0, 9.0});

	return knownObjs;
}

bool LDS::matchKnownObjects(Object& detectedObject, const std::vector<Object>& objectCat) {
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


std::string LDS::LDSDataToString(bool bool_result) {

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

std::vector<std::string> LDS::passLDSData(Object& detectedObject) {
	// Need to pass trajectory data and threat type (or guess of threat type)
	std::vector<std::string> ldsData;
	bool detectionResult = handleDetection(detectedObj.getposVel()[0], detectedObj.getposVel()[1], detectedObject);
	std::string lDS_Data_result = this->LDSDataToString(detectionResult);

	ldsData.push_back(lDS_Data_result);

	ldsData.push_back("Trajectory: " + std::to_string(detectedObj.getposVel()[0]) + "," + std::to_string(detectedObject.getposVel()[1]));
	ldsData.push_back(std::string("Threat Type: ") + (detectionResult == true ? "Known" : "Unknown"));

	return ldsData;
}


float detectionThreshold = 10.0;
bool LDS_fail = false; // Hide the current status of LDS operation, never show to the operator - they will never know if the thing is broken or not

std::vector<Object> knownObjs;

std::vector<float> latlon = get_ll("lla.txt");
Object detectedObject1 = Object::Object("Object 1", { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 });


