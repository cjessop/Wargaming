#include <vector>
#include <string>
#include <algorithm>

class Object
{
public:
    Object() {}  // Default constructor
    ~Object() {}

    // Main constructor
    Object(std::string name, std::vector<double> posVel) {}

    std::string getName() const { return name; }
    const std::vector<double>& getposVel() const { return posVel; }

private:
    std::string name;
    std::vector<double> posVel;
};

