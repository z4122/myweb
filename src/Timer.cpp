#include"Timer.hpp"

int Timer::addTimerEvery(int interval_s,int interval_ms){
    int fd = 0;
    
    gettimeofday(&current_time,NULL);

    time_value.it_value.tv_sec = current_time.tv_sec;
    time_value.it_value.tv_nsec = 0;
    time_value.it_interval.tv_sec = interval_s;
    time_value.it_interval.tv_nsec = 1000*interval_ms;

    fd = timerfd_create(CLOCK_REALTIME,O_NONBLOCK);  

    timerfd_settime(fd,TFD_TIMER_ABSTIME,&time_value,NULL);

    return fd;
}

int Timer::addTimerAfter(int time_s,int time_ms){
    int fd = 0;
    
    gettimeofday(&current_time,NULL);

    time_value.it_value.tv_sec = current_time.tv_sec+time_s;
    time_value.it_value.tv_nsec = 1000000*time_ms;
    time_value.it_interval.tv_sec = 0;
    time_value.it_interval.tv_nsec = 0;

    fd = timerfd_create(CLOCK_REALTIME,O_NONBLOCK);

    timerfd_settime(fd,TFD_TIMER_ABSTIME,&time_value,NULL);

    return fd;
}
int addTimerAt(){

    return 0;
}

bool Timer::delTimer(int fd){
    return true;

}

void Timer::changeTimer(int fd,int time_s,int time_ms,int interval_s,int interval_ms){
    gettimeofday(&current_time,NULL);

    time_value.it_value.tv_sec = current_time.tv_sec+time_s;
    time_value.it_value.tv_nsec = 1000000*time_ms;
    time_value.it_interval.tv_sec = interval_s;
    time_value.it_interval.tv_nsec = interval_ms;

    timerfd_settime(fd,TFD_TIMER_ABSTIME,&time_value,NULL);
}

void Timer::addHeapValue(int fd){
    gettimeofday(&current_time,NULL);
    
    current_time.tv_sec+=7;
    timer_heap.push(make_pair(current_time,fd));
}

void Timer::popHeapValue(){
    timer_heap.pop();
}

pair<struct timeval,int> Timer::topHeapValue(){
    pair<struct timeval,int> temp = timer_heap.top();
    return temp;
}

bool Timer::isHeapEmpty(){
    if(timer_heap.empty())
        return true;
    return false;
}