#ifndef TREE_H_
#define TREE_H_

#include "General.h"
#include "GameState.h"
#include "Node.h"
#include "Move.h"


struct Tree
{
    int MAXTURNS;
    Move zMove;
    double MAXTIME;

    GameState startState;

    Node *tree;
    queue<Node*> extendableNodes;

    Tree();

    void clear();

    void createTree(const GameState &StartState);
    void makeChildren(Node *cNode);

    void getMoves(vector<Move> &moves);
};

#endif //TREE_H_
