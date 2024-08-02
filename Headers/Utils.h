#pragma once
#include <string>
#include <stdio.h>
#include <iostream>
#include <vector>

float readTime(const std::string& trajFile);

std::string readRadar(const std::string& radFile);
float countdown(float& seconds);
float check_progress(float current_time);
std::vector<float> get_ll(std::string lla_file);
std::vector<Object> readCatalogue(const std::string& catalogueFile);
double generate_random_number(float num1, float num2);
int generate_random_int(int num1, int num2);


