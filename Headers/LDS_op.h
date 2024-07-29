#pragma once
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

	std::vector<double> latlon;
	std::vector<Object> objectCatalogue;
	Object detectedObj;

	void setTime(float t);
	float getTime();
	void verifyTime();
	std::vector<Object> createCatalogue();
	void evaluate_progress(float current_time);
	bool fail_noFail();
	bool handleDetection(float lat, float lon, Object& detectedObject);
	std::vector<Object> generateCatalogueLDS();
	bool matchKnownObjects(Object& detectedObject, const std::vector<Object>& objectCat);
	std::string LDSDataToString(bool bool_result);
	std::vector<std::string> passLDSData(Object& detectedObject);

private:
	float time;
	float detectionThreshold;
	bool LDS_fail;
	std::vector<Object> knownObjs;


};
