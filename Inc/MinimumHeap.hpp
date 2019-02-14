#pragma once
#include<vector>
#include<sys/time.h>
using namespace std;



class MinimumHeap{
public:
    MinimumHeap();
    ~MinimumHeap();

    void addValue(int fd);
    pair<struct timeval,int> popTop();

private:
    vector<pair<struct timeval,int>> timer_heap;
};