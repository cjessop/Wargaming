#include <iostream>
#include <math.h>
#include <vector>
#include <stdio.h>
#include "../Headers/Utils.h"
#include <random>
#include "../Headers/Object.h"
#include "../Headers/LDS_op.h"
#include "../Headers/EWR.h"

float total_time = readTime("Trajec.txt");

float seconds = total_time;


static void run() {
	/*LDS LDS_op;
	EWR EWR_op;
	*/
	while (seconds > 0) {
		seconds = countdown(seconds);
		float progress = check_progress(seconds);
	}
}