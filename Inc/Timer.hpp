#include<iostream>
#include<sys/timerfd.h>
#include<sys/time.h>
#include<stdint.h>
#include<unistd.h>
#include<fcntl.h>
#include<queue>

using namespace std;

class Timer{
    public:
        Timer(){};
        ~Timer(){};

        int addTimerEvery(int interval_s,int interval_ms);
        int addTimerAfter(int time_s,int time_ms);
        int addTimerAt();
        bool delTimer(int fd);
        void changeTimer(int fd,int time_s,int time_ms,int interval_s,int interval_ms);
        void addHeapValue(int fd);
        bool isHeapEmpty();
        void popHeapValue();
        pair<struct timeval,int> topHeapValue();

    private:
        struct itimerspec time_value;
        struct timeval current_time;
        struct cmp{
            bool operator()(pair<struct timeval,int> a,pair<struct timeval,int> b){
                if(a.first.tv_sec>b.first.tv_sec)
                    return true;
                return false;
            }
        };
        priority_queue<pair<struct timeval,int>,vector<pair<struct timeval,int>>,cmp> timer_heap;
};

