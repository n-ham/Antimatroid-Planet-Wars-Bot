/*
    General functions to simplify things
*/
#ifndef GENERAL_H_
#define GENERAL_H_

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>

#include <sys/time.h>


using namespace std;



template <class V>
V minn(V a, V b)
{
    if(a < b)
        return a;
    return b;
};



template <class V>
V maxx(V a, V b)
{
    if(a > b)
        return a;
    return b;
};

//returns the index of the maximum value
template <class V>
V amaxx(V a, V b)
{
    if(a >= b)
        return 1;
    return 2;
};


//adds elements from one vector to another
template <class V>
vector<V>& operator+=(vector<V> &A, const vector<V> &B)
{
    for(int i=0; i<(int)B.size(); i++)
        A.push_back(B[i]);
    return A;
}




#endif //GENERAL_H_
