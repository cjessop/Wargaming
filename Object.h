#pragma once
#include <vector>
#include <string>
#include <algorithm>


class Object
{
public:
	Object() {}
	~Object();

	Object(std::string name, std::vector<double> posVel) {}

	std::string getName() const { return name; }
	const std::vector<double>& getposVel() const { return posVel; }


	std::string name;
	std::vector<double> posVel;
private:
	
};
