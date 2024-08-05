#pragma once
#include <math.h>
#include <vector>
#include <stdio.h>
#include <random>
#include "Discriminator.h"
#include "Utils.h"
#include "EWR.h"
#include "LDS_op.h"
#include "Object.h"

class Controller
{

public:

	Controller();
	~Controller();
	void run(std::string trajector_file);

private:
	float control_total_time = 0;
	float control_seconds = control_total_time;
	LDS LDS_op;
	EWR EWR_op;
	Discriminator discrim_controller;

};
