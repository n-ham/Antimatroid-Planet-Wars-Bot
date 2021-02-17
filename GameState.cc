#include "GameState.h"

void GameState::findClosestFriendsAndEnemies()
{
    int cdist, fdist, edist, destination;
    bool foundClosestFriend, foundClosestEnemy;

    for(int source=0; source<noPlanets; source++)
    {
        foundClosestEnemy = foundClosestFriend = 0;

        fdist = edist = maxdistance;

        for(int c=1; c<noPlanets; c++)
        {
            destination = planets[source].closest[c];
            cdist = dist[source][destination];

            if(futureStates[destination][maxdistance].owner == 1 && !foundClosestFriend)
            {
                foundClosestFriend = 1;
                fdist = cdist;
            }
            if (futureStates[destination][maxdistance].owner == 2 && !foundClosestEnemy)
            {
                foundClosestEnemy = 1;
                edist = cdist;
            }

            if(foundClosestEnemy && foundClosestFriend)
                break;
        }
        planets[source].closestNFE[1] = fdist;
        planets[source].closestNFE[2] = edist;
    }
};


void GameState::generateFutureStates(int source, int startTime)
{
    int cOwner, minPlayer, maxPlayer;;
    Fleet cFleet;

    //resets information and deals with the first states
    if(startTime == 0)
    {
        futureStates[source][0].owner = planets[source].owner;
        futureStates[source][0].ships = maxx(0, planets[source].ships - futureStates[source][0].outgoingShips[planets[source].owner]);
        startTime++;
    }

    //calculates the states
    for(int t=startTime; t<=maxdistance; t++)
    {
        futureStates[source][t].owner = futureStates[source][t-1].owner;
        futureStates[source][t].ships = futureStates[source][t-1].ships;

        cOwner = futureStates[source][t].owner;

        if(cOwner > 0)
            futureStates[source][t].ships += planets[source].growth;

        //deals with incoming fleets
        incoming[1] = futureStates[source][t].incomingShips[1];
        incoming[2] = futureStates[source][t].incomingShips[2];
        if(incoming[1] > 0 || incoming[2] > 0)   //some fleets have arrived at the planet
        {
            if(cOwner > 0)   //planet is us
            {
                futureStates[source][t].ships += (incoming[cOwner] - incoming[(cOwner%2)+1]);
                if(futureStates[source][t].ships < 0)
                {
                    futureStates[source][t].ships = -futureStates[source][t].ships;
                    cOwner = futureStates[source][t].owner = (cOwner%2)+1;
                }
            }
            else //planet is neutral
            {
                //works out which player has the most ships on their fleets arriving at time j
                maxPlayer = amaxx(incoming[1], incoming[2]);
                minPlayer = (maxPlayer%2)+1;

                if(futureStates[source][t].ships >= incoming[maxPlayer])
                    futureStates[source][t].ships -= incoming[maxPlayer];
                else if(incoming[1] == incoming[2])
                    futureStates[source][t].ships = 0;
                else
                {
                    cOwner = futureStates[source][t].owner = maxPlayer;
                    futureStates[source][t].ships = incoming[maxPlayer] - maxx(futureStates[source][t].ships, incoming[minPlayer]);
                }
            }
        }
        //sends outgoing fleets
        futureStates[source][t].ships = maxx(0, futureStates[source][t].ships - futureStates[source][t].outgoingShips[cOwner]);
    }
};

void GameState::updateFutureStates()
{
    for(int source=0; source<noPlanets; source++)
    {
        if(timeChanged[source] < 1000)
        {
            generateFutureStates(source, timeChanged[source]);
            generateUsableShips(source);
            generateSendableShips(source, timeChanged[source]);
            timeChanged[source] = 1000;
        }
    }
};

void GameState::generateFutureStates()
{
    for(int source=0; source<noPlanets; source++)
    {
       generateFutureStates(source, 0);
       generateUsableShips(source);
       generateSendableShips(source, 0);
       timeChanged[source] = 1000;
    }
};


void GameState::generateUsableShips(int source)
{
    int cOwner;

    //leaves ships there
    if(futureStates[source][maxdistance].owner > 0)
        futureStates[source][maxdistance].usableShips = futureStates[source][maxdistance].ships;

    //tracks back to make sure we leave ships that are needed
    for(int t=maxdistance-1; t>=0; t--)
    {
        cOwner = futureStates[source][t].owner;
        if(cOwner > 0)
        {
            if(futureStates[source][t].owner == futureStates[source][t+1].owner)
                futureStates[source][t].usableShips = minn(futureStates[source][t].ships, futureStates[source][t+1].usableShips);
            else
                futureStates[source][t].usableShips = 0;
        }
    }

    //sets up the first state if it's starting as a neutral planet
    if(planets[source].owner == 0)
        futureStates[source][0].usableShips = 0;


    //does a final scan to calculate the number of ships that can actually be sent from each time point
    int ouships=0, nuships = futureStates[source][0].usableShips;
    for(int t=1; t<=maxdistance; t++)
    {
        cOwner = futureStates[source][t].owner;

        if(futureStates[source][t-1].owner == cOwner)
            ouships = nuships;
        else
            ouships = 0;
        nuships = futureStates[source][t].usableShips;

        if(cOwner > 0)
        {
            if(futureStates[source][t-1].owner == cOwner)
                futureStates[source][t].usableShips = maxx(0, nuships - ouships);
        }
        else if(cOwner == 0)
            ouships = nuships = futureStates[source][t].usableShips = 0;
    }
};




void GameState::generateSendableShips(int source, int startTime)
{
    //sets up the first state if the planet isn't owned by us
    if(startTime == 0)
    {
        if(planets[source].owner > 0)
            futureStates[source][0].sendableShips = futureStates[source][0].ships;
        else if(planets[source].owner == 0)
            futureStates[source][0].sendableShips = 0;
        startTime++;
    }

    for(int t=startTime; t<=maxdistance; t++)
    {
        if(futureStates[source][t].owner == 0)
            futureStates[source][t].sendableShips = 0;
        else if(futureStates[source][t].owner == futureStates[source][t-1].owner)
            futureStates[source][t].sendableShips = maxx(0, futureStates[source][t].ships - futureStates[source][t-1].ships);
        else
            futureStates[source][t].sendableShips = futureStates[source][t].ships;
    }
};

void GameState::generateCounts()
{
    finalGrowthCount[0] = finalGrowthCount[1] = finalGrowthCount[2] = 0;
    finalShipCount[0] = finalShipCount[1] = finalShipCount[2] = 0;

    for(int source=0; source<noPlanets; source++)
    {
        finalGrowthCount[futureStates[source][maxdistance].owner] += planets[source].growth;
        finalShipCount[futureStates[source][maxdistance].owner] += futureStates[source][maxdistance].ships;
    }
};

double GameState::getPayoff()
{
    return finalShipCount[1] - finalShipCount[2] + (MAXTURNS - turn - maxdistance)*(finalGrowthCount[1] - finalGrowthCount[2]);
};

double GameState::getFinalShips(int player)
{
    return finalShipCount[player] + (MAXTURNS - turn - maxdistance)*(finalGrowthCount[player]);
};






bool GameState::shouldExpand(int player)
{
    if(turn == 0)
        return 1;
    if(finalGrowthCount[player] > 2*finalGrowthCount[(player%2)+1])
        return 0;

    return 1;
};



void GameState::pickMove(int player, int destination, Move &move)
{
    int minPlayer, maxPlayer;
    int source, cdist, tsent, tadded, extraAmount, usableShips, sentShips, claimAmount;
    priority_queue<Fleet> usableFleets;
    Fleet cFleet;

    FutureState nstate(futureStates[destination][0].owner, futureStates[destination][0].ships), ostate;


    extraAmount = usableShips = move.payoff =  0;
    for(int t=1; t<maxdistance; t++)
    {
        if(getTime() > MAXTIME)
        {
            move.fleets.clear();
            return;
        }

        ostate = nstate;

        //updates the new state of the destination planet
        if(ostate.owner > 0)
            nstate.ships += planets[destination].growth;

        incoming[1] = futureStates[destination][t].incomingShips[1];
        incoming[2] = futureStates[destination][t].incomingShips[2];
        if(incoming[1] > 0 || incoming[2] > 0)  //some fleets have arrived at the planet
        {
            if(nstate.owner > 0)   //planet isn't neutral
            {
                nstate.ships += (incoming[nstate.owner] - incoming[(nstate.owner%2)+1]);
                if(nstate.ships < 0)
                {
                    nstate.ships = -nstate.ships;
                    nstate.owner = (nstate.owner%2)+1;
                }
            }
            else //planet is neutral
            {
                maxPlayer = amaxx(incoming[1], incoming[2]);
                minPlayer = (maxPlayer%2)+1;

                if(nstate.ships >= incoming[maxPlayer])
                    nstate.ships -= incoming[maxPlayer];
                else if(incoming[1] == incoming[2])
                    nstate.ships = 0;
                else
                {
                    nstate.owner = maxPlayer;
                    nstate.ships = incoming[maxPlayer] - maxx(nstate.ships, incoming[minPlayer]);
                }
            }
        }
        //sends outgoing fleets
        nstate.ships = maxx(0, nstate.ships - futureStates[destination][t].outgoingShips[nstate.owner]);


        //adds new fleets that can be made by us
        for(int c=1; c<noPlanets; c++)
        {
            source = planets[destination].closest[c];
            cdist = dist[source][destination];
            tadded = t-cdist;
            tsent  = tadded;

            if(tadded<0 )
                break;

            if(futureStates[source][tadded].owner == player  && futureStates[source][tadded].usableShips > 0 )
            {
                usableFleets.push(Fleet(player, source, destination, cdist, tadded, tsent, t, futureStates[source][tadded].usableShips));
                usableShips += futureStates[source][tadded].usableShips;
            }
            else if(futureStates[source][tadded].owner == (player%2)+1)
                extraAmount += futureStates[source][tadded].sendableShips;

        }


        //works out the amount needed to be sent
        if(nstate.owner == player && nstate.ships<extraAmount)
            claimAmount = extraAmount - nstate.ships;
        else if(nstate.owner == (player%2)+1 && ostate.owner == player)
            claimAmount = nstate.ships+extraAmount;
        else if(nstate.owner == (player%2)+1 && ostate.owner == (player%2)+1)
            claimAmount = nstate.ships+extraAmount+1;
        else
            claimAmount = 0;


        sentShips = 0;
        if(claimAmount > 0 && usableShips >= claimAmount)
        {
            //creates the move there
            while(sentShips < claimAmount)
            {
                cFleet = usableFleets.top();
                usableFleets.pop();

                //works out when the send the fleet
                cdist = dist[cFleet.source][destination];

                cFleet.tsent = t-cdist;
                cFleet.tarrive = t;

                if(cFleet.ships +sentShips >= claimAmount)
                {
                    if(cFleet.ships+sentShips > claimAmount)
                        usableFleets.push(Fleet(player, cFleet.source, cFleet.destination, cFleet.distance, cFleet.tadded, cFleet.tsent, cFleet.tarrive, cFleet.ships +sentShips -claimAmount));
                    move.fleets.push_back(Fleet(player, cFleet.source, cFleet.destination, cFleet.distance, cFleet.tadded, cFleet.tsent, cFleet.tarrive,  claimAmount-sentShips));
                    sentShips += claimAmount - sentShips;
                    move.ships += sentShips;

                    extraAmount = 0;
                }
                else
                {
                    move.fleets.push_back(cFleet);
                    sentShips += cFleet.ships;
                }
            }
            usableShips -= sentShips;
            move.payoff -= sentShips;


            //sends our ships
            if(nstate.owner == player)
                nstate.ships += sentShips;
            else if(nstate.owner == (player%2)+1)
                nstate.ships -= sentShips;
            else
                nstate.ships = nstate.ships + incoming[(player%2)+1] - sentShips;

            if(nstate.ships < 0)
            {
                nstate.ships = -nstate.ships;
                nstate.owner = player;
            }
        }
        if(nstate.owner == player && ostate.owner != 0)
            move.payoff += 2*planets[destination].growth;
        else if(nstate.owner == (player%2)+1 && ostate.owner != 0 )
            move.payoff -= 2*planets[destination].growth;
    }
    if(nstate.owner == (player%2)+1)
        move.payoff += (2*(MAXTURNS-turn-maxdistance)*planets[destination].growth);
    else if(nstate.owner == player)
        move.payoff -= (2*(MAXTURNS-turn-maxdistance)*planets[destination].growth);
};

void GameState::pickExpansionaryMove(int player, int destination, Move &eMove)
{
    int minPlayer, maxPlayer;
    int source, cdist, tsent, tadded, extraAmount, usableShips, sentShips, claimAmount;
    priority_queue<Fleet> usableFleets;
    Fleet cFleet;

    FutureState nstate(futureStates[destination][0].owner, futureStates[destination][0].ships), ostate;

    extraAmount = usableShips = eMove.payoff = 0;
    for(int t=1; t<maxdistance; t++)
    {
        if(getTime() > MAXTIME)
        {
            eMove.fleets.clear();
            return;
        }

        ostate = nstate;

        //updates the new state of the destination planet
        if(ostate.owner > 0)
            nstate.ships += planets[destination].growth;
        else if(nstate.ships >= (MAXTURNS-(turn+t))*planets[destination].growth) //planet costs more than it will even generate
            return;


        incoming[1] = futureStates[destination][t].incomingShips[1];
        incoming[2] = futureStates[destination][t].incomingShips[2];

        if(incoming[1] > 0 || incoming[2] > 0)  //some fleets have arrived at the planet
        {
            if(nstate.owner > 0)   //planet is us
            {
                nstate.ships += (incoming[nstate.owner] - incoming[(nstate.owner%2)+1]);
                if(nstate.ships < 0)
                {
                    nstate.ships = -nstate.ships;
                    nstate.owner = (nstate.owner%2)+1;
                }
            }
            else //planet is neutral
            {
                //works out which player has the most ships on their fleets arriving at time t
                maxPlayer = amaxx(incoming[1], incoming[2]);
                minPlayer = (maxPlayer%2)+1;

                if(nstate.ships >= incoming[maxPlayer])
                    nstate.ships -= incoming[maxPlayer];
                else if(incoming[1] == incoming[2])
                    nstate.ships = 0;
                else
                {
                    nstate.owner = maxPlayer;
                    nstate.ships = incoming[maxPlayer] - maxx(nstate.ships, incoming[minPlayer]);
                }
            }
        }

        //sends outgoing fleets
        nstate.ships = maxx(0, nstate.ships - futureStates[destination][t].outgoingShips[nstate.owner]);


        //adds new fleets that can be made by us
        for(int c=1; c<noPlanets; c++)
        {
            source = planets[destination].closest[c];
            cdist = dist[source][destination];
            tadded = t-cdist;
            tsent  = tadded;

            if(tadded<0 )
                break;

            if(nstate.owner > 0 && futureStates[source][tadded].owner == player )
                usableShips += futureStates[source][tadded].usableShips;
            else if(futureStates[source][tadded].owner == player  && futureStates[source][tadded].usableShips > 0 )
            {
                usableFleets.push(Fleet(player, source, destination, cdist, tadded, tsent, t, futureStates[source][tadded].usableShips));
                usableShips += futureStates[source][tadded].usableShips;
            }
            else if(futureStates[source][tadded].owner == (player%2)+1)
                extraAmount += futureStates[source][tadded].sendableShips;
        }

        //does a mock attack
        if(nstate.owner > 0)
        {
            if(nstate.owner == player && ostate.owner != 0)
            {
                nstate.ships += (usableShips - extraAmount);
                usableShips = extraAmount = 0;

            }
            else if(nstate.owner == (player%2)+1)
            {
                nstate.ships += (extraAmount - usableShips);
                usableShips = extraAmount = 0;
            }
            if(nstate.ships < 0)
            {
                nstate.ships = -nstate.ships;
                nstate.owner = (nstate.owner%2)+1;
            }
        }
        if(nstate.owner == (player%2)+1)
        {
            eMove.fleets.clear();
            return;
        }

        //works out the amount needed to be sent
        if(nstate.owner == 0 && incoming[(player%2)+1] == 0)
        {
            if(extraAmount == usableShips && nstate.ships <= planets[destination].growth)
                claimAmount = maxx(nstate.ships+1, extraAmount);
            else
                claimAmount = nstate.ships + extraAmount + 1;
        }
        else
            claimAmount = 0;


        sentShips = 0;
        if(claimAmount > 0 && usableShips >= claimAmount)
        {
            //creates the move there
            while(sentShips < claimAmount)
            {
                cFleet = usableFleets.top();
                usableFleets.pop();

                cdist = dist[cFleet.source][destination];
                cFleet.tsent = t-cdist;
                cFleet.tarrive = t;

                if(cFleet.ships +sentShips >= claimAmount)
                {
                    if(cFleet.ships+sentShips > claimAmount)
                        usableFleets.push(Fleet(player, cFleet.source, cFleet.destination, cFleet.distance, cFleet.tadded, cFleet.tsent, cFleet.tarrive, cFleet.ships +sentShips -claimAmount));
                    eMove.fleets.push_back(Fleet(player, cFleet.source, cFleet.destination, cFleet.distance, cFleet.tadded, cFleet.tsent, cFleet.tarrive,  claimAmount-sentShips));
                    sentShips += claimAmount - sentShips;
                }
                else
                {
                    eMove.fleets.push_back(cFleet);
                    sentShips += cFleet.ships;
                }
            }
            usableShips -= sentShips;
            eMove.ships += sentShips;
            eMove.payoff = (MAXTURNS-turn-t)*planets[destination].growth - sentShips;

            extraAmount = 0;

            while(!usableFleets.empty())
                usableFleets.pop();

            nstate.ships -= sentShips;
            if(nstate.ships < 0)
            {
                nstate.ships = -nstate.ships;
                nstate.owner = player;
            }
        }
    }
};

void GameState::pickMoves(int player, int maxMoves, vector<Move> &moves)
{
    int destination;
    priority_queue<Move> moveQueue;
    for(int p=0; p<noPlanets && getTime() < MAXTIME; p++)
    {
        destination = bestPlanets[p];

        Move cMove;
        cMove.rank = planets[destination].rank;

        if(futureStates[destination][maxdistance].owner == 0)
        {
            if(shouldExpand(player) && planets[destination].growth > 0 && (double)planets[destination].closestNFE[player] < 1.5*(double)planets[destination].closestNFE[(player%2)+1])
                pickExpansionaryMove(player, destination, cMove);
        }
        else if(player > 0)
            pickMove(player, destination, cMove);

        if(isValid(cMove))
            moveQueue.push(cMove);

        if((int)moveQueue.size() == maxMoves)
            break;
    }

    moves.clear();
    while(!moveQueue.empty())
    {
        moves.push_back(moveQueue.top());
        moveQueue.pop();
    }
};


void GameState::pickDefensiveMove(int player, int destination, Move &defenseMove)
{
    int minPlayer, maxPlayer;
    int source, cdist, tsent, tadded, usableShips, sentShips, claimAmount, tclaimed;
    queue<Fleet> usableFleets;
    Fleet cFleet;

    FutureState nstate(futureStates[destination][0].owner, futureStates[destination][0].ships), ostate;


    usableShips =  0;
    tclaimed = 0;
    for(int t=1; t<maxdistance; t++)
    {
        if(getTime() > MAXTIME)
        {
            defenseMove.fleets.clear();
            return;
        }

        ostate = nstate;

        //updates the new state of the destination planet
        if(ostate.owner > 0)
        {
            nstate.ships += planets[destination].growth;

            if(!tclaimed)
                tclaimed = t;
        }


        incoming[1] = futureStates[destination][t].incomingShips[1];
        incoming[2] = futureStates[destination][t].incomingShips[2];

        if(incoming[1] > 0 || incoming[2] > 0)   //some fleets have arrived at the planet
        {
            if(nstate.owner > 0)   //planet is us
            {
                nstate.ships += (incoming[nstate.owner] - incoming[(nstate.owner%2)+1]);
                if(nstate.ships < 0)
                {
                    nstate.ships = -nstate.ships;
                    nstate.owner = (nstate.owner%2)+1;
                }
            }
            else //planet is neutral
            {
                //works out which player has the most ships on their fleets arriving at time j
                maxPlayer = amaxx(incoming[1], incoming[2]);
                minPlayer = (maxPlayer%2)+1;

                if(nstate.ships >= incoming[maxPlayer])
                    nstate.ships -= incoming[maxPlayer];
                else if(incoming[1] == incoming[2])
                    nstate.ships = 0;
                else
                {
                    nstate.owner = maxPlayer;
                    nstate.ships = incoming[maxPlayer] - maxx(nstate.ships, incoming[minPlayer]);
                }
            }
        }
        //sends outgoing fleets
        nstate.ships = maxx(0, nstate.ships - futureStates[destination][t].outgoingShips[nstate.owner]);


        //adds new fleets that can be made by us
        for(int c=1; c<noPlanets; c++)
        {
            source = planets[destination].closest[c];
            cdist = dist[source][destination];
            tadded = t-cdist;
            tsent  = tadded;

            if(tadded<0 )
                break;

            if(futureStates[source][tadded].owner == player  && futureStates[source][tadded].usableShips > 0)
            {
                usableFleets.push(Fleet(player, source, destination, cdist, tadded, tsent, t, futureStates[source][tadded].usableShips));
                usableShips += futureStates[source][tadded].usableShips;
            }
        }


        //works out the amount needed to be sent
        if(nstate.owner == (player%2)+1 && ostate.owner == player)
            claimAmount = nstate.ships;
        else
            claimAmount = 0;


        sentShips = 0;
        if(claimAmount > 0 && usableShips > claimAmount)
        {
            //creates the move there
            while(sentShips < claimAmount)
            {
                cFleet = usableFleets.front();
                usableFleets.pop();

                cdist = dist[cFleet.source][destination];
                cFleet.tsent = t-cdist;
                cFleet.tarrive = t;

                if(cFleet.ships +sentShips >= claimAmount)
                {
                    if(cFleet.ships+sentShips > claimAmount)
                        usableFleets.push(Fleet(player, cFleet.source, cFleet.destination, cFleet.distance, cFleet.tadded, cFleet.tsent, cFleet.tarrive, cFleet.ships +sentShips -claimAmount));
                    defenseMove.fleets.push_back(Fleet(player, cFleet.source, cFleet.destination, cFleet.distance, cFleet.tadded, cFleet.tsent, cFleet.tarrive,  claimAmount-sentShips));
                    sentShips += (claimAmount - sentShips);
                    defenseMove.ships += sentShips;
                }
                else
                {
                    defenseMove.fleets.push_back(cFleet);
                    sentShips += cFleet.ships;
                }
            }
            usableShips -= sentShips;

            //sends our ships
            if(nstate.owner == player)
                nstate.ships += sentShips;
            else if(nstate.owner == (player%2)+1)
                nstate.ships -= sentShips;
            else
                nstate.ships = nstate.ships + incoming[(nstate.owner%2)+1] - sentShips;

            //updates the state
            if(nstate.ships < 0)
            {
                nstate.ships = -nstate.ships;
                nstate.owner = player;
            }
        }
    }
};

void GameState::pickDefensiveMoves(int player, vector<Move> &defenseMoves)
{
    int destination;

    defenseMoves.clear();
    for(int p=0; p<noPlanets; p++)
    {
        destination = bestPlanets[p];
        Move cMove;

        pickDefensiveMove(player, destination, cMove);
        if(isValid(cMove))
        {
            defenseMoves.push_back(cMove);
            addMove(cMove);
        }
    }
};


int GameState::pickRedistributionPlanet(const Fleet &cFleet)
{
    int bdist, rdist, cdist, sdist, cPlanet, redistPlanet;

    bdist = 9999;
    redistPlanet = -1;

    for(int c=1; c<noPlanets; c++)
    {
        cPlanet = planets[cFleet.source].closest[c];

        cdist = dist[cFleet.source][cPlanet];
        sdist = dist[cPlanet][cFleet.destination];

        if(futureStates[cPlanet][cdist].owner == 1 && cdist + sdist <= cFleet.tarrive &&  cdist + planets[cPlanet].closestNFE[2] < bdist && planets[cPlanet].closestNFE[2] < planets[cFleet.source].closestNFE[2])
        {
            rdist = cdist;
            bdist = cdist + planets[cPlanet].closestNFE[2];
            redistPlanet = cPlanet;
        }
    }

    return redistPlanet;
}


void GameState::pickRedistributiveMove(int source, Move &rMove)
{
    int bdist, rdist, cdist, destination, redistPlanet, shipsToSend;

    bdist = 9999;
    redistPlanet = -1;

    for(int c=1; c<noPlanets; c++)
    {
        destination = planets[source].closest[c];
        cdist = dist[source][destination];

        if(cdist > planets[source].closestNFE[2])
            break;

        if(futureStates[destination][cdist].owner == 1 && cdist + planets[destination].closestNFE[2] < bdist && planets[destination].closestNFE[2] <= planets[source].closestNFE[2])
        {
            rdist = cdist;
            bdist = cdist + planets[destination].closestNFE[2];
            redistPlanet = destination;
        }
    }
    shipsToSend = minn(futureStates[source][0].usableShips, 4*planets[source].growth);
    if(redistPlanet >= 0)
        rMove.fleets.push_back(Fleet(1, source, redistPlanet, rdist, 0, 0, rdist,  shipsToSend));
};

void GameState::pickRedistributiveMoves(vector<Move> &redistMoves)
{
    int source;

    redistMoves.clear();
    for(int p=noPlanets-1; p>=0 && getTime() < 0.80; p--)
    {
        source = bestPlanets[p];
        Move cMove;

        if(planets[source].owner == 1 && futureStates[source][0].usableShips > 0)
            pickRedistributiveMove(source, cMove);

        if(isValid(cMove))
        {
            redistMoves.push_back(cMove);
            addMove(cMove);
        }
    }
};


bool GameState::isValid(const Fleet &cFleet)
{
    if(cFleet.owner <1 || cFleet.owner > 2 || cFleet.source < 0 || cFleet.source >= noPlanets || cFleet.destination < 0 || cFleet.destination >= noPlanets)
        return 0;
    return 1;
};


bool GameState::isValid(const Move &move)
{
    if((int)move.fleets.size() == 0)
        return 0;
    for(int f=0; f<(int)move.fleets.size(); f++)
        if(!isValid(move.fleets[f]))
            return 0;
    return 1;
};



void GameState::addMove(const Move &move)
{
    if(!isValid(move))
        return;

    Fleet cFleet;
    for(int f=0; f<(int)move.fleets.size() && getTime() < 0.9; f++)
    {
        cFleet = move.fleets[f];

        futureStates[cFleet.source][cFleet.tadded].usableShips -= cFleet.ships;
        futureStates[cFleet.source][cFleet.tadded].outgoingShips[cFleet.owner] += cFleet.ships;
        futureStates[cFleet.destination][cFleet.tarrive].incomingShips[cFleet.owner] += cFleet.ships;

        if(cFleet.tadded < timeChanged[cFleet.source])
            timeChanged[cFleet.source] = cFleet.tadded;
        if(cFleet.tarrive < timeChanged[cFleet.destination])
            timeChanged[cFleet.destination] = cFleet.tarrive;
    }
};

void GameState::addMoves(vector<Move> &moves)
{
    for(int m=0; m<(int)moves.size(); m++)
        addMove(moves[m]);
}


void GameState::makeMove(const Move &move)
{
    int redistPlanet, cdist;
    Fleet cFleet;
    if(isValid(move))
    {
        for(int f=0; f<(int)move.fleets.size() && getTime() < 0.96; f++)
        {
            cFleet = move.fleets[f];
            if(cFleet.owner == 1 && cFleet.tadded == 0 && cFleet.source != cFleet.destination)
            {
                cdist = dist[cFleet.source][cFleet.destination];

                if(cFleet.tsent == 0)
                    IssueOrder(cFleet.source, cFleet.destination, cFleet.ships);
                else if(futureStates[cFleet.destination][cdist].owner == 2 && futureStates[cFleet.destination][cdist-1].owner > 0)
                    IssueOrder(cFleet.source, cFleet.destination, cFleet.ships);
                else if(futureStates[cFleet.destination][cdist].owner == 1 && planets[cFleet.source].closestNFE[2] > planets[cFleet.destination].closestNFE[2])
                    IssueOrder(cFleet.source, cFleet.destination, cFleet.ships);
                else if(getTime() < 0.92)
                {
                    redistPlanet = pickRedistributionPlanet(cFleet);
                    if(redistPlanet >= 0)
                        IssueOrder(cFleet.source, redistPlanet, cFleet.ships);
                }
            }
        }
    }
};

void GameState::makeMoves(vector<Move> &moves)
{
    for(int m=0; m<(int)moves.size(); m++)
        makeMove(moves[m]);
};

void GameState::IssueOrder(int source, int destination, int ships)
{
    if(ships > 0 && planets[source].owner == 1 && planets[source].ships >= ships && source != destination)
    {
        planets[source].ships -= ships;
        cout << source << " " << destination << " " << ships <<  endl;
        cout.flush();
    }
};


double GameState::getTime()
{
    gettimeofday(&timer, NULL);
    currentTime = timer.tv_sec+(timer.tv_usec/1000000.0);
    return currentTime-startTime;
}

int GameState::Distance(int source, int destination)
{
    double dx = planets[source].x - planets[destination].x;
    double dy = planets[source].y - planets[destination].y;
    return (int)ceil(sqrt(dx * dx + dy * dy));
};



void GameState::resetInformation()
{
    for(int source=0; source<(int)futureStates.size(); source++)
    {
        for(int t=0; t<(int)futureStates[source].size(); t++)
        {
            futureStates[source][t].incomingShips[1] = futureStates[source][t].incomingShips[2] = 0;
            futureStates[source][t].outgoingShips[1] = futureStates[source][t].outgoingShips[2] = 0;
        }
    }
};

void GameState::ParseGameState(stringstream &gameStream)
{
    queue<Fleet> newFleets;

    int Owner, Ships, Growth, Source, Destination, TLength, TArrive;
    double X, Y;
    char lineType;
    string cString;

    //gets the information from the string stream
    int id = 0;
    planetCount[0] = planetCount[1] = planetCount[2] = 0;
    growthCount[0] = growthCount[1] = growthCount[2] = 0;
    while(gameStream >> lineType && getTime() < 0.25)
    {
        if(lineType == 'P')
        {
            gameStream >> X >> Y >> Owner >> Ships >> Growth;
            getline(gameStream, cString);

            if(turn == 0) //sets up that planet
                planets.push_back(Planet(id++, Owner, Ships, Growth, X, Y));
            else //updates the information for that planet
            {
                planets[id].owner = Owner;
                planets[id].ships = Ships;
                id++;
            }

            planetCount[Owner]++;
            growthCount[Owner] += Growth;
        }
        else if(lineType == 'F')
        {
            gameStream >> Owner >> Ships >> Source >> Destination >> TLength >> TArrive;
            newFleets.push(Fleet(Owner, Source, Destination, 0, 0, 0, TArrive, Ships));
        }
    }

    //does stuff for the first turn
    if(turn == 0)
    {
        noPlanets = planets.size();
        dist = vector<vector<int> >(noPlanets, vector<int>(noPlanets, 0));
        maxdistance = 0;

        priority_queue<vector<int> > bestPlanetQueue;
        vector<int> planetVec(3, 0);
        vector<priority_queue<pair<int, int> > > planetQueues(noPlanets, priority_queue<pair<int, int> >());
        for(int source=0; source<noPlanets; source++)
        {
            planetVec[0] = planets[source].growth;
            planetVec[1] = -planets[source].avNeighbourDistance;
            planetVec[2] = source;
            bestPlanetQueue.push(planetVec);

            for(int destination=0; destination<=source; destination++)
            {
                dist[source][destination] = dist[destination][source] = Distance(source, destination);
                maxdistance = maxx(maxdistance, dist[source][destination]);

                planetQueues[source].push(pair<int, int>(-dist[source][destination], destination));
                if(source != destination)
                    planetQueues[destination].push(pair<int, int>(-dist[source][destination], source));

                planets[source].avNeighbourDistance += dist[source][destination];
                planets[destination].avNeighbourDistance += dist[source][destination];
            }
        }

        for(int r=0; r<noPlanets; r++)
        {
            bestPlanets.push_back(bestPlanetQueue.top()[2]);
            planets[bestPlanets[r] ].rank = r;
            bestPlanetQueue.pop();
        }

        futureStates = vector<vector<FutureState> >(noPlanets, vector<FutureState>(maxdistance+1, FutureState()));
        for(int source=0; source<noPlanets; source++)
        {
            planets[source].avNeighbourDistance /= (double)(noPlanets-1);
            planets[source].closestNFE = vector<int>(3, maxdistance);

            while(!planetQueues[source].empty())
            {
                planets[source].closest.push_back(planetQueues[source].top().second);
                planetQueues[source].pop();
            }
        }
        planetQueues.clear();

        timeChanged = vector<int>(noPlanets, 1000);
        incoming = vector<int>(3, 0);
    }

    //adds the fleets
    Fleet newFleet;
    while(!newFleets.empty())
    {
        newFleet = newFleets.front();
        futureStates[newFleet.destination][newFleet.tarrive].incomingShips[newFleet.owner] += newFleet.ships;
        newFleets.pop();
    }
};




