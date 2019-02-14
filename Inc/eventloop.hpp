#pragma once
#include "epoll.hpp"
#include "Log.hpp"

class Eventloop{
public:
    Eventloop(int num,Log *log);

    void start();

private:
    Epoll epoll_;


};