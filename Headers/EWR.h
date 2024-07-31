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

	bool matchObj(Object& detectedObject, const std::vector<Object>& objectCat);
	std::string getObjData(const std::vector<double>& ldsData) const;
	void addObject(const std::string& name, const std::vector<double>& posVel);
	bool matchObject(const std::vector<double>& ldsData);
	std::vector<Object> Catalogue::GenerateCatalogueEWR();

private:
	std::vector<Object> knownObjects;
	std::vector<std::string> lds_data;
	
};

class Detection
{
public:
	Detection();
	~Detection();

	double getFenceProba(const std::vector<double>& ldsData);
	double getVolumeProba(const std::vector<double>& ldsData);
	std::string posVelToString(const std::vector<double>& posVel);

private:

};

class EWR
{
public:
	EWR();
	~EWR();

	void processExternalObs(const std::vector<double>& obs);
	void processOffNavData(const std::vector<double>& offNavData);
	void handleDetection(const std::vector<double>& ldsData, Catalogue& catalogue, Detection& detection);
	bool isSimilar(Object& detectedObject, const std::vector<Object>& objectCat);
	bool matchObject(const std::vector<double>& ldsData);
	void processMatchedObj(const std::vector<double>& ldsData, Catalogue& catalogue);
	void processUnmatchedObj(const std::vector<double>& ldsData, double fence, double volume);
	std::string processLDSdata(std::vector<std::string>& ldsData);
	double calculateProba(const std::string& objData);
	double calculateProbaUnmatched(double fenceProba, double volumeProba);
	std::string guessObj(double fence, double volume);
	void getThreatParams(const std::string& objData, double& altitude, double& velocity, double& range);
	void passDataToDiscriminator(const std::string& data);
	void passDataToDisplay(const std::string& data);
	void passDataToTrajectory(double altitude, double velocity, double range);

	Detection detectionHandler;
	Catalogue objectCatalogue;

private:
	

};
