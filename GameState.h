#ifndef GAMESTATE_H_
#define GAMESTATE_H_

#include "General.h"

#include "Planet.h"
#include "FutureState.h"
#include "Fleet.h"
#include "Move.h"



struct GameState
{
    /*
        Variables
    */
    int turn, noPlanets, maxdistance, MAXTURNS;
    double MAXTIME;
    vector<vector<int> > dist;

    vector<int> bestPlanets;
    vector<Planet> planets;
    vector<vector<FutureState> > futureStates; //futureStates[planet][time]
    vector<int> timeChanged, incoming; //timeChanged[planet]

    vector<double> planetCount, growthCount, finalGrowthCount, finalShipCount;

    //the timer
    timeval timer;
    double startTime, currentTime;


    /*
        Functions
    */
    GameState(){};

    void findClosestFriendsAndEnemies();

    void generateFutureStates(int source, int startTime);
    void updateFutureStates();
    void generateFutureStates();

    void generateUsableShips(int source);
    void generateSendableShips(int source, int startTime);

    void generateCounts();

    double getPayoff();
    double getFinalShips(int player);


    bool shouldExpand(int player);
    void pickMove(int player, int destination, Move &move);
    void pickExpansionaryMove(int player, int destination, Move &eMove);
    void pickMoves(int player, int maxMoves, vector<Move> &moves);

    void pickDefensiveMove(int player, int destination, Move &defenseMove);
    void pickDefensiveMoves(int player, vector<Move> &defenseMoves);


    int pickRedistributionPlanet(const Fleet &cFleet);
    void pickRedistributiveMove(int source, Move &rMove);
    void pickRedistributiveMoves(vector<Move> &redistMoves);


    bool isValid(const Fleet &cFleet);
    bool isValid(const Move &move);

    void addMove(const Move &move);
    void addMoves(vector<Move> &moves);

    void makeMove(const Move &move);
    void makeMoves(vector<Move> &moves);
    void IssueOrder(int source, int destination, int ships);

    double getTime();
    int Distance(int source, int destination);

    void resetInformation();
    void ParseGameState(stringstream &gameStream);

};

ofstream& operator<<(ofstream &o, const GameState &printState);


#endif //GAMESTATE_H_
