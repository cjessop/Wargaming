#pragma once
#include <string>
#include <stdio.h>
#include <iostream>
#include <vector>

float readTime(std::string trajFile);

float countdown(float& seconds);

float check_progress(float current_time);

std::vector<float> get_ll(std::string lla_file);

double generate_random_number(float num1, float num2);