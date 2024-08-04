#include <vector>
#include <string>
#include <algorithm>
#include "Object.h"



Object::Object() : name(""), posVel() {
};  // Default constructor
Object::~Object() {
};

// Main constructor
Object::Object(std::string name, std::vector<double> posVel)
	: name(std::move(name)), posVel(std::move(posVel)) {}

std::string Object::getName() const { return name; }
const std::vector<double>& Object::getposVel() const { return posVel; }

std::string name;
std::vector<double> posVel;


