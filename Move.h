#ifndef MOVE_H_
#define MOVE_H_

#include "General.h"
#include "Fleet.h"

struct Move
{
    int payoff, ships, rank;
    vector<Fleet> fleets;

    Move()
    {
        payoff = ships = rank = 0;
    };

    bool operator<(const Move &b) const
    {
        if(this->payoff < b.payoff)
            return 1;
        else if(this->payoff > b.payoff)
            return 0;

        if(this->rank > b.rank)
            return 1;
        else if(this->rank < b.rank)
            return 0;

        return 0;
    };

};



#endif //MOVE_H_
