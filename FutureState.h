#ifndef FUTURESTATE_H_
#define FUTURESTATE_H_

#include "General.h"
#include "Fleet.h"

struct FutureState
{
    int ships, usableShips, sendableShips, owner;
    vector<int> incomingShips, outgoingShips;

    FutureState()
    {
        owner = ships = usableShips = 0;
        incomingShips = outgoingShips = vector<int>(3, 0);
    };

    FutureState(int Owner, int Ships)
    {
        owner = Owner;
        ships = Ships;
        incomingShips = outgoingShips = vector<int>(3, 0);
    };
};

#endif //FUTURESTATE_H_
