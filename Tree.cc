#include "Tree.h"

Tree::Tree()
{

}

void Tree::createTree(const GameState &StartState)
{
    startState = StartState;
    MAXTURNS = startState.MAXTURNS;
    MAXTIME = startState.MAXTIME;

    //sets up the initial node
    tree = new Node(startState);
    extendableNodes.push(tree);


    Node *cNode;

    while(startState.getTime() < MAXTIME && !extendableNodes.empty())
    {
        cNode = extendableNodes.front();
        extendableNodes.pop();

        makeChildren(cNode);
    }

    //evaluates the nodes in the tree
    tree->evaluateNode();
}

void Tree::clear()
{
    while(!extendableNodes.empty())
        extendableNodes.pop();
    if(tree)
        tree->clear();
}


void Tree::getMoves(vector<Move> &moves)
{
    Node *cNode = tree;

    moves.clear();
    while(cNode->bestChild >=0)
    {
        cNode = cNode->children[cNode->bestChild];
        if(cNode->parent->player == 1)
            moves.push_back(cNode->moveToGetHere);
    }
}



void Tree::makeChildren(Node *cNode)
{
    vector<Move> newMoves;
    int bestPayoff;
    Node *newKid, *bestKid;

    cNode->currentState.pickMoves(cNode->player, 10, newMoves);
    newMoves.push_back(zMove);


    if(cNode->player == 1)
    {
        bestPayoff = -999999999;
        for(int c=0; c<(int)newMoves.size(); c++)
        {
            newKid = new Node(cNode, newMoves[c]);
            makeChildren(newKid);

            if(newKid->evaluateNode() > bestPayoff)
            {
                bestKid = newKid;
            }

            cNode->addChild(bestKid);
            if(cNode->children[0]->moveToGetHere.fleets.size() > 0 || cNode->children[0]->children[0]->moveToGetHere.fleets.size() > 0)
                extendableNodes.push(cNode->children[0]->children[0]);
        }
    }
    else
    {
        //picks the players favourite move
        for(int c=0; c<(int)newMoves.size(); c++)
            cNode->addChild(new Node(cNode, newMoves[c]));
    }


}


