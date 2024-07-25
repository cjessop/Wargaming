#include "../Headers/Utils.h"
#include "../Headers/Controller.h"
#include "../Headers/LDS_op.h"
#include "../Headers/Object.h"
#include <vector>

Controller control;

int main() {
	float total_time = readTime("Trajec.txt");
	

	float seconds = total_time;

	while (seconds > 0) {
		//seconds = countdown(seconds);
		//float progress = check_progress(seconds);
		
		control.run();

	}

	return 0;
}