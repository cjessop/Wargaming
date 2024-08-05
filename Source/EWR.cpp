#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <algorithm>
#include "Utils.h"
#include "EWR.h"
#include "LDS_op.h"
#include "Object.h"
#include "EWR.h"

LDS LDS_op_EWR;

Detection::Detection() {
};

Detection::~Detection() {
};


std::vector<Object> knownObjects;

std::string Detection::posVelToString(const std::vector<double>& posVel) {
	std::string result = "[";
	for (size_t i = 0; i < posVel.size(); i++) {
		if (i > 0) result += ", ";
		result += std::to_string(posVel[i]);
	}

	result += "]";
	return result;
}


double Detection::getFenceProba(const std::vector<double>& ldsData) {
	return generate_random_number(ldsData[0], ldsData[1]);
}

double Detection::getVolumeProba(const std::vector<double>& ldsData) {
	return generate_random_number(ldsData[2], ldsData[3]);
}


// This class represents the system controller (operator) for the EWR user player
EWR::EWR() {

}

EWR::~EWR()
 {

 }

Object DetectedObjectFromLDS = LDS_op_EWR.detectedObj;
std::vector<std::string> lds_data = LDS_op_EWR.passLDSData(DetectedObjectFromLDS);

void EWR::addObject(const std::string& name, const std::vector<double>& posVel) {
	knownObjects.emplace_back(name, posVel);
}

std::string EWR::getObjData(const std::vector<double>& ldsData) const {
	return "object data";

}

/*Method to generate the catalogue specific to the EWR operator. Conventionally this would be
identical to that of the LDS catalogue, or read from a file (future)*/

std::vector<Object> EWR::GenerateCatalogueEWR() {
	knownObjs.emplace_back("Object 1", std::vector<double>{1.0, 2.0, 3.0});
	knownObjs.emplace_back("Object 2", std::vector<double>{4.0, 7.0, 9.0});

	return knownObjs;
}

/*Function to perform an independant check on match/no match against its own (EWR) catalogue*/
bool EWR::matchObj(Object& detectedObject, const std::vector<Object>& objectCat) {
	return true;
}


std::string EWR::processLDSdata(std::vector<std::string>& ldsData) {
	// Call function from LDS_op class to check for fail case of the system
	bool ldsfail_nofail = LDS_op_EWR.fail_noFail();
	std::string s_fail = "fail";
	std::string s_true = "true";

	if (ldsfail_nofail == false) {
		for (int i = 0; i < ldsData.size(); i++) {
			if (ldsData[i].find(s_fail) != std::string::npos) {
				std::cout << "Irregular object passed from LDS system" << std::endl;
				return ldsData[i];
			}
		}
	}
}

void EWR::processExternalObs(const std::vector<double>& obs) {
	// Take data from the EWR radar and process it correctly. Return positions and velocities?
	

}

void EWR::processOffNavData(const std::vector<double>& offNavData) {
	// more here
}



void EWR::handleDetection(const std::vector<double>& ldsData, std::vector<Object>& catalogue, Detection& detection) {
	bool match = matchObject(catalogue);

	double fence = detection.getFenceProba(ldsData);
	double volume = detection.getVolumeProba(ldsData);

	if (!match) {
		processUnmatchedObj(ldsData, fence, volume);
	}
	else {
		processMatchedObj(ldsData);
	}
}

/*Perform a simple check to see if the two evaluated objects are "similar". How is similar defined??*/
bool EWR::isSimilar(Object& detectedObject, const std::vector<Object>& objectCat) {
	bool match = false;

	//for (const auto& obj : objectCat) { // Use reference to object to allow the object itself to be altered
	for (int i = 0; i < objectCat.size(); i++) {
		if (detectedObject.getposVel() == objectCat[i].getposVel() || detectedObject.getName() == objectCat[i].getName()) {
	
		//if (obj == detectedObject) {
			std::cout << "Detected signature matches known signature from EWR catalogue and is safe" << std::endl;
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

/*Create a function to perform an object match test, using the isSimilar function */
bool EWR::matchObject(const std::vector<Object>& objectCat) {
	for (Object& obj : knownObjects) {
		if (isSimilar(obj, objectCat)) {
			return true;
		}
		else {
			return false;
		}
	}
}

void EWR::processMatchedObj(const std::vector<double>& ldsData) {
	std::string objData = getObjData(ldsData);
	double proba = calculateProba(objData);
	double altitude, velocity, range;

	getThreatParams(objData, altitude, velocity, range);


	// Section below is for passing to future GUI elements

	passDataToDiscriminator(objData);
	passDataToDisplay(objData);
	passDataToTrajectory(altitude, velocity, range);
	
}


void EWR::processUnmatchedObj(const std::vector<double>& ldsData, double fence, double volume) {
	double proba = calculateProbaUnmatched(fence, volume);
	std::string guessedObj = guessObj(fence, volume);

	passDataToDiscriminator(guessedObj);
	passDataToDisplay(guessedObj);
}

double EWR::calculateProba(const std::string& objData) {
	// Unfuck this function

	return 0.8; //Placeholder value
}

double EWR::calculateProbaUnmatched(double fenceProba, double volumeProba) {
	// This function needs fleshing out so that we can actually do useful shit with it
	
	return 0.4;
}

std::string EWR::guessObj(double fence, double volume) {
	// This function is terribly implemented and once again, needs an overhaul
	return "Currently Unknown Object";
}

void EWR::getThreatParams(const std::string& objData, double& altitude, double& velocity, double& range) {
	// Placeholder values used below. Remove the alt, vel, and pos arguments and take everything from object?
	altitude = 1000.0;
	velocity = 500.0;
	range = 10000.0;
}

void EWR::passDataToDiscriminator(const std::string& data) {
	// Pass data to Discriminator component
}

void EWR::passDataToDisplay(const std::string& data) {
	// Pass data to Display component
}

void EWR::passDataToTrajectory(double altitude, double velocity, double range) {
	// Pass data to Trajectory component
}

