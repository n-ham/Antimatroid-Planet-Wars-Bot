#ifndef FLEET_H_
#define FLEET_H_

#include "General.h"


struct Fleet
{
    int owner, ships, source, destination, distance, tadded, tsent, tarrive;

    Fleet() {};

    Fleet(int Owner, int Source, int Destination, int Distance, int Tadded, int Tsent, int Tarrive, int Ships)
    {
        owner = Owner;
        source = Source;
        destination = Destination;
        distance = Distance;
        tadded = Tadded;
        tsent = Tsent;
        tarrive = Tarrive;
        ships = Ships;
    };

    bool operator<(const Fleet &a) const
    {
        //uses fleets from the closest planets first
        if(this->distance > a.distance)
            return 1;
        else if(this->distance < a.distance)
            return 0;

        //uses fleets from as early as possible
        if(this->tadded < a.tadded)
            return 1;
        else if(this->tadded > a.tadded)
            return 0;

        return 0;
    };

};

#endif //FLEET_H_
