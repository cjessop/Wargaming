#pragma once
#include <math.h>
#include <vector>
#include <stdio.h>
#include <random>
#include <LDS_op.h>
#include <EWR.h>
#include "Object.h"
#include "LDS_op.h"
#include "EWR.h"
#include "Utils.h"


class Discriminator
{
public:
	Object ThreatObject;
	EWR EWR_op;
	LDS LDS_op;

	bool ThreatStatusFromEWR();
	bool ThreatStatusFromLDS(bool& LDS_fail);

	std::vector<std::string> runDiscrim(bool b_threat, Object& object);

private:

};