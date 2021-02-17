#include <iostream>


#include "General.h"
#include "GameState.h"
#include "Tree.h"

GameState gameState;
Tree gameTree;
vector<Move> moves, newMoves;


void pickMoves()
{

    //picks our first round defense moves
    gameState.pickDefensiveMoves(1, newMoves);
    moves += newMoves;


    //picks our main moves
    gameState.updateFutureStates();
    gameState.findClosestFriendsAndEnemies();

    gameTree.clear();
    gameTree.createTree(gameState);
    gameTree.getMoves(newMoves);


    moves += newMoves;
    gameState.addMoves(newMoves);


    //picks our redistribution moves
    if(gameState.turn > 3)
    {
        gameState.updateFutureStates();
        gameState.pickRedistributiveMoves(newMoves);
        moves += newMoves;
    }

}

string current_line;
stringstream mapStream(stringstream::in | stringstream::out);

int main(int argc, char *argv[])
{
    cout.sync_with_stdio(0);

    gameState.turn = 0;
    gameState.MAXTURNS = 199;
    gameState.MAXTIME = 0.75;
    gameState.planetCount = gameState.growthCount = gameState.finalGrowthCount = gameState.finalShipCount = vector<double>(3, 0);


    while(getline(cin, current_line))
    {
        if(current_line.size() > 0 && current_line[0] == 'g')
        {
            gettimeofday(&gameState.timer, NULL);
            gameState.startTime = gameState.timer.tv_sec+(gameState.timer.tv_usec/1000000.0);



            //updates the game state
            gameState.resetInformation();
            gameState.ParseGameState(mapStream);
            gameState.generateFutureStates();
            gameState.generateCounts();
            gameState.findClosestFriendsAndEnemies();
            mapStream.clear();

            //picks our moves for the turn
            if(gameState.planetCount[1] > 0)
            {
                moves.clear();
                pickMoves();
                gameState.makeMoves(moves);
            }


            gameState.turn++;



            //finishes the turn
            cout << "go" << endl;
            cout.flush();


        }
        else if(current_line.size() > 0 && (current_line[0] == 'P' || current_line[0] == 'F'))
        {
            current_line += "\n";
            mapStream << current_line;
        }
        current_line = "";
    }



    return 0;
}
