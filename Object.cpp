#include <vector>
#include <string>
#include <algorithm>

class Object
{
public:
	Object();
	~Object();


    Object(std::string name, std::vector<double> characteristics)
        : name(name), psoVel(posVel) {}

    std::string getName() const { return name; }
    const std::vector<double>& getposVel() const { return posVel; }

private:

    std::string name;
    std::vector<double> posVel;
};

Object::Object()
{
}

Object::~Object()
{
}