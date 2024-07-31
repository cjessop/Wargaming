#pragma once
#include <math.h>
#include <vector>
#include <stdio.h>
#include <random>
#include <LDS_op.h>
#include <EWR.h>
#include <Object.h>

class Discriminator
{
private:

    //Declare classes required for the discriminator class
    EWR EWR_op;
    LDS LDS_op;
    Object ThreatObject;



public:
    Discriminator(/* args */);
    ~Discriminator();
};


    bool ThreatStatusFromEWR();
    bool ThreatStatusFromLDS();
    std::vector<std::string> runDiscrim();

Discriminator::Discriminator(/* args */)
{
}

Discriminator::~Discriminator()
{
}
