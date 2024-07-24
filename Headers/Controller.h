#pragma once
#include <math.h>
#include <vector>
#include <stdio.h>
#include "../Headers/Utils.h"
#include <random>
#include "../Headers/Object.h"
#include "../Headers/LDS_op.h"
#include "../Headers/EWR.h"

class Controller
{
public:
	Controller();
	~Controller();

	float control_total_time = readTime("Trajec.txt");
	float control_seconds = control_total_time;

	void run();

private:

};
