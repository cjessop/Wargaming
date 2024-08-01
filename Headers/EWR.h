#pragma once
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <algorithm>
#include "Object.h"
#include "Discriminator.h"

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

	void addObject(const std::string& name, const std::vector<double>& posVel);
	std::string getObjData(const std::vector<double>& ldsData) const;
	std::vector<Object> GenerateCatalogueEWR();
	void processExternalObs(const std::vector<double>& obs);
	void processOffNavData(const std::vector<double>& offNavData);
	bool matchObj(Object& detectedObject, const std::vector<Object>& objectCat);
	void handleDetection(const std::vector<double>& ldsData, std::vector<Object>& catalogue, Detection& detection);
	bool isSimilar(Object& detectedObject, const std::vector<Object>& objectCat);
	bool matchObject(const std::vector<Object>& objectCat);
	void processMatchedObj(const std::vector<double>& ldsData);
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
	Object DetectedObjectFromLDS;

private:
	std::vector<Object> knownObjs;
	std::vector<std::string> lds_data;

};
