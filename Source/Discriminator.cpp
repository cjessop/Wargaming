#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <algorithm>
#include "Utils.h"
#include "EWR.h"
#include "LDS_op.h"
#include "Object.h"

EWR EWR_op;
LDS LDS_op;

Object ThreatObject = EWR_op.DetectedObjectFromLDS;

bool Discriminator::ThreatStatusFromEWR() {
    bool b_threat;

    if (EWR_op.isSimilar() == true) {
        return b_threat = true;
    }
    else if (EWR_op.isSimilar() == false) {
        return b_threat = false;
    }
}

bool Discriminator::ThreatStatusFromLDS(bool& LDS_fail) {
    bool b_threat;

    if (LDS_fail == false) {    
        if (LDS_op.isSimilar() == true) {
            return b_threat = true;
        }
        else if (LDS_op.isSimilar() == false) {
            return b_threat = false;
        }
    }
}

std::vector<std::string> Discriminator::runDiscrim(bool b_threat, Object& object) {

    std::cout << "Initialising discriminator" << std::endl;

    // de is a placeholder for something else
    // Declare all member variables
    int threat = 1;
    int non_threat = 2;
    int de = 3;
    int strange = 4;
    double proba_discrim_fail = generate_random_number(0.0, 1.0); // Dice rolls are shit but will have to do for now
    double rand_proba_discrim = generate_random_number(0.0, 1.0);
    int rand_int = generate_random_int(1, 4);
    std::string failCase;
    bool m_EWR = ThreatStatusFromEWR();

    // Set discrimination threshold as 20% for now
    if (proba_discrim_fail < 0.2) {

        // Handle (true) threats
        if (b_threat == true) {
            std::vector<double> ThreatPosVel = object.getposVel();

            if (ThreatPosVel[0] >= 5.0) {
                if (rand_proba_discrim > 0.5) {
                    // Place holder value for the first element of the posVel vector 
                    std::cout << "Unidentified object is of type " << std::to_string(threat) << std::endl;
                    std::cout << "Confidence is: " << std::to_string(rand_proba_discrim) << std::endl;

                    return object.getName() + std::to_string(threat);
                }
                else if (rand_proba_discrim < 0.4) {
                    // Assign incorrect discriminator label to the object (simulating a failure of the discriminator)
                    // Sometimes the correct label will be assigned as 1/4 chance (number of classes is 4 currently)
                    std::cout << "Unidentified object is of type " << std::to_string(rand_int) << std::endl;
                    std::cout << "Confidence is: " << std::to_string(rand_proba_discrim) << std::endl;

                    return object.getName() + std::to_string(rand_int);
                }
            }
            else if (ThreatPosVel[0] < 0.5 && ThreatPosVel[0] > 0.2) {
                // Handle de case
                if (rand_proba_discrim > 0.2) {
                    std::cout << "Unidentified object is of type " << std::to_string(de) << std::endl;

                    return Object.getName() + std::to_string(de);
                }
                else if (rand_proba_discrim <= 0.2) {
                    std::cout << "Discriminator failure" << std::endl;

                    return failCase = "Failed discrimination";
                }
            }
            else if (!ThreatPosVel[0]) {
                // Handle case that the first element of the array in not possible to discern (i.e. the object is strange)

                return object.getName() + std::to_string(strange);
            }
        }
        else if (b_threat == false) {
            std::vector<double> NonThreatPosVel = object.getposVel();
            // Perform some simple discrimination check

        }
    }
}

Discriminator::Discriminator() {};

Discriminator::~Discriminator() {};


