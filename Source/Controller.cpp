#include <iostream>
#include <math.h>
#include <stdio.h>
#include <random>
#include "Controller.h"


Controller::Controller() {};

Controller::~Controller() {};

void Controller::run(std::string trajectory_file) {

	float control_total_time = readTime(trajectory_file);

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

