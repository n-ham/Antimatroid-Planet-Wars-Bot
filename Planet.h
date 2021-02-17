#ifndef PLANET_H_
#define PLANET_H_

#include "General.h"

struct Planet
{
    int id, owner, ships, growth, rank;
    double x, y;

    double avNeighbourDistance;
    vector<int> closest;
    vector<int> closestNFE;


    Planet(int ID, int Owner, int Ships, int Growth, double X, double Y)
    {
        ID = ID;
        owner = Owner;
        ships = Ships;
        growth = Growth;
        x = X;
        y = Y;
    };
};

#endif //PLANET_H_

