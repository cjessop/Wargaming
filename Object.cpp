#include <vector>
#include <string>
#include <algorithm>

class Object
{
public:
	Object();
	~Object();


    Object(std::string name, std::vector<double> characteristics)
        : name(name), characteristics(characteristics) {}

    std::string getName() const { return name; }
    const std::vector<double>& getCharacteristics() const { return characteristics; }

private:

    std::string name;
    std::vector<double> characteristics;

};

Object::Object()
{
}

Object::~Object()
{
}