#ifndef NODE_H_
#define NODE_H_

#include "General.h"
#include "GameState.h"

struct Node
{
    /*
        Variables
    */
    int layer, player, noChildren, bestChild;
    double payoff;

    GameState currentState;

    Move moveToGetHere;

    Node *parent;
    vector<Node*> children;


    /*
        Functions
    */
    Node()
    {
        noChildren = 0;
    };

    Node(const GameState &StartState)
    {
        noChildren = layer = payoff = 0;
        player = 1;
        bestChild = -1;

        parent = NULL;
        currentState = StartState;
        moveToGetHere = Move();

        currentState.generateCounts();
    };

    Node(Node *Parent, Move &newMove)
    {
        noChildren = payoff = 0;
        bestChild = -1;

        parent = Parent;
        moveToGetHere = newMove;

        layer = parent->layer+1;
        player = (parent->player%2)+1;

        //updates information about the state
        currentState = parent->currentState;

        if(currentState.isValid(newMove))
        {
            currentState.addMove(newMove);
            currentState.updateFutureStates();
            currentState.generateCounts();
        }
    };

    void clear()
    {
        parent = NULL;
        for(int c=0; c<(int)children.size(); c++)
            children[c]->clear();
        delete this;
    };

    void addChild(Node* newChild)
    {
        children.push_back(newChild);
        noChildren++;
    };


    double evaluateNode()
    {
        if(noChildren > 0) //decision node
        {
            double currentPayoff = 0.0;

            if(player == 1)
            {
                payoff = -99999999;
                for(int c=0; c<noChildren; c++)
                {
                    currentPayoff = children[c]->evaluateNode();
                    if(currentPayoff > payoff)
                    {
                        payoff = currentPayoff;
                        bestChild = c;
                    }
                }
            }
            else if(player == 2)
            {
                payoff = 99999999;
                for(int c=0; c<noChildren; c++)
                {
                    currentPayoff = children[c]->evaluateNode();
                    if(currentPayoff < payoff)
                    {
                        payoff = currentPayoff;
                        bestChild = c;
                    }
                }
            }
            return payoff;
        }
        else    //terminal node
        {
            bestChild = -1;


            payoff = currentState.getPayoff();

            if(currentState.finalGrowthCount[1] == 0)
                payoff = -999999;
            else if(currentState.finalGrowthCount[2] == 0)
                payoff =  999999;
            else if(payoff == 0)
                payoff = currentState.getFinalShips(1);
            else if(payoff > 0)
                payoff += 50000;
            else
                payoff -= 50000;

            return payoff;
        }
        return 0.0;
    };



};

#endif //NODE_H_
