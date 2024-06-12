#include "Utils.h"
#include <vector>

int main() {
	float total_time = readTime("Trajec.txt");


	float seconds = total_time;

	while (seconds > 0) {
		seconds = countdown(seconds);
		float progress = check_progress(seconds);
	}

	return 0;
}