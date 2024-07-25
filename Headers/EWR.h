#pragma once
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <algorithm>
#include "Object.h"

class Catalogue
{
public:
	Catalogue();
	~Catalogue();

	bool matchObj(const std::vector<double>& ldsData) {};
	std::string getObjData(const std::vector<double>& ldsData) {};
	void addObject(const std::vector<double>& ldsData) {};
	bool matchObject(const std::vector<double>& ldsData) {};

private:
	std::vector<Object> knownObjects;
	std::vector<std::string> lds_data;
	bool isSimilar(const std::vector<double>& a, const std::vector<double>& b) const {};
	std::string posVeltoString(const std::vector<double>& posVel) const {};

};

class Detection
{
public:
	Detection();
	~Detection();

	double getFenceProba(const std::vector<double>& ldsData) {};
	double getVolumeProba(const std::vector<double>& ldsData) {};

private:

};


class EWR
{
public:
	EWR();
	~EWR();

	void processLDSdata(const std::vector<double>& ldsData) {};
	void processExternalObs(const std::vector<double>& obs) {};
	void processOffNavData(const std::vector<double>& offNavData) {};

	Detection detectionHandler;
	Catalogue objectCatalogue;

private:
	void handleDetection(const std::vector<double>& ldsData, Catalogue& catalogue, Detection& detection) {};
	void processMatchedObj(const std::vector<double>& ldsData, const Catalogue& catalogue) {};
	void processUnmatchedObj(const std::vector<double>& ldsData, double fence, double volume) {};
	double calculateProba(const std::string& objData) {};
	double calculateProbaUnmatched(double fenceProba, double volumeProba) {};
	std::string guessObj(double fence, double volume) {};
	void getThreatParams(const std::string& objData, double& altitude, double& velocity, double& range) {};
	void passDataToDiscriminator(const std::string& data) {};
	void passDataToDisplay(const std::string& data) {};
	void passDataToTrajectory(double altitude, double velocity, double range) {};

};
