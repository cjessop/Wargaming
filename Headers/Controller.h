#pragma once
#include <math.h>
#include <vector>
#include <stdio.h>
#include <random>

class Controller
{
public:
	Controller();
	~Controller();
	
	void run(std::string trajector_file);

private:
	float control_total_time = 0;
	float control_seconds = control_total_time;

};
