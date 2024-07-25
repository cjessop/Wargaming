#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <algorithm>
#include "../Headers/Utils.h"
#include "../Headers/EWR.h"
#include "../Headers/LDS_op.h"

// Default contructor for the Catalogue class
Catalogue::Catalogue() {

}

// Default destructor for the Catalogue class
Catalogue::~Catalogue() {

}

bool Catalogue::matchObj(const std::vector<double>& ldsData) {
	return true;
}

std::string Catalogue::getObjData(const std::vector<double>& ldsData) const {
	return "object data";

}

/*
Catalogue() {
	knownObjects.emplace_back("Object 1", std::vector<double>{1.0, 2.0, 3.0});
	knownObjects.emplace_back("Object 1", std::vector<double>{4.0, 7.0, 9.0});
}*/

void Catalogue::addObject(const std::string& name, const std::vector<double>& posVel) {
	knownObjects.emplace_back(name, posVel);
}

// Create a function to perform an object match test, using the isSimilar function
bool Catalogue::matchObject(const std::vector<double>& ldsData) const {
	for (const auto& obj : knownObjects) {
		if (isSimilar(obj.getposVel(), ldsData)) {
			return true;
		}
	}
}


Detection::Detection() {
};

Detection::~Detection() {
};


std::vector<Object> knownObjects;

bool Detection::isSimilar(const std::vector<double>& a, const std::vector<double>& b) const {
	// Perform a simple check to see if the two evaluated objects are "similar". How is similar defined??
	if (a.size() != b.size()) return false;
	for (size_t i = 0; i < a.size(); i++) {
		if (std::abs(a[i] - b[i]) > 0.1) return false;
	}
	return true;
}

std::string Detection::posVelToString(const std::vector<double>& posVel) const {
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

std::vector<std::string> lds_data = LDS::passLDSData();

std::vector<std:: EWR::processLDSdata(const std::vector<double>& ldsData) {
	// Call function from LDS_op class to check for fail case of the system
	bool ldsfail_nofail = LDS::fail_noFail();
	std::string s_fail, s_true = "fail", "true";

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



void EWR::handleDetection(const std::vector<double>& ldsData, Catalogue& catalogue, Detection& detection) {
	bool match = catalogue.matchObj(ldsData);

	double fence = detection.getFenceProba(ldsData);
	double volume = detection.getVolumeProba(ldsData);

	if (!match) {
		processUnmatchedObj(ldsData, fence, volume);
	}
	else {
		processMatchedObj(ldsData, catalogue);
	}
}


void EWR::processMatchedObj(const std::vector<double>& ldsData, const Catalogue& catalogue) {
	std::string objData = catalogue.getObjData(ldsData);
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
	altitude = 1000.0f;
	velocity = 500.0f;
	range = 10000.0f;
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
