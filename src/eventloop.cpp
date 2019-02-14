#include "eventloop.hpp"


Eventloop::Eventloop(int num,Log *log):
                epoll_(num,log)
{

}

void Eventloop::start(){
    while(1){
        epoll_.detectEvent();
    }

}