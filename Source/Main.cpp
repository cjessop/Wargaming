#include "Utils.h"
#include "Controller.h"
#include "LDS_op.h"
#include "Object.h"

Controller control;

int main() {
	float total_time = readTime("Trajec.txt");
	

	float seconds = total_time;

	while (seconds > 0) {
		//seconds = countdown(seconds);
		//float progress = check_progress(seconds);
		
		control.run("lla.txt");

	}

	return 0;
}