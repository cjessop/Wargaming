#pragma once
#include <string>
#include <stdio.h>
#include <iostream>
#include <vector>
#include "Object.h"

float readTime(const std::string& trajFile);

std::string readRadar(const std::string& radFile);
float countdown(float& seconds);
float check_progress(float current_time);
std::vector<float> get_ll(std::string lla_file);
std::vector<std::string> readCatalogue(const std::string& catalogueFile);
std::vector<Object> BuildObjectCatalogue(const std::string& CatFile);
double generate_random_number(float num1, float num2);
int generate_random_int(int num1, int num2);


