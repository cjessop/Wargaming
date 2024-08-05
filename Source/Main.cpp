#include "Utils.h"
#include "Controller.h"
#include "Object.h"

Controller control;

int main() {
	float total_time = readTime("Trajec.txt");


	while (total_time > 0) {
		//seconds = countdown(seconds);
		//float progress = check_progress(seconds);
		
		control.run("lla.txt");

	}

	return 0;
}