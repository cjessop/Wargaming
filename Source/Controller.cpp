#include <iostream>
#include <math.h>
#include <vector>
#include <stdio.h>
#include "../Headers/Utils.h"
#include <random>
#include "../Headers/Object.h"
#include "../Headers/LDS_op.h"
#include "../Headers/EWR.h"
#include "../Headers/Controller.h"

Controller::Controller() {

}

Controller::~Controller() {

}


void Controller::run(std::string trajector_file) {

	float control_total_time = readTime(trajector_file);

	LDS LDS_op;
	EWR EWR_op;

	while (control_seconds > 0) {
		control_seconds = countdown(control_seconds);
		float progress = check_progress(control_seconds);
		std::cout << progress;
		if (progress < 0.2) {
			continue;
		}
		else if (progress >= 0.2) {
			LDS_op.setTime(control_seconds);
			float Lds_time = LDS_op.getTime();
			std::cout << Lds_time;
		}
	}
}
