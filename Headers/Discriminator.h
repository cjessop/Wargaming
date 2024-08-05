#pragma once
#include <math.h>
#include <vector>
#include <stdio.h>
#include <random>
#include "Object.h"
#include "Utils.h"

class LDS;
class EWR;

class Discriminator
{
	
public:
	Discriminator(); //Default constructor
	~Discriminator(); //Defaul destructor
	

	bool ThreatStatusFromEWR();
	bool ThreatStatusFromLDS(bool& LDS_fail);

	std::string runDiscrim(bool b_threat, Object& object);

private:
	Object ThreatObject;
	EWR* EWR_op;
	LDS* LDS_op;
};