#pragma once
#include<unistd.h>
#include<stdio.h>
#include<assert.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string.h>

#include<sys/resource.h>
#include<sys/epoll.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>

#include"ThreadPoll.hpp"
#include"HandleRequest.hpp"
#include"Log.hpp"
#include"Timer.hpp"

using namespace std;

class Epoll{
public:
    //构造函数，监听事件的最大数目
    Epoll(int num,Log *log);
    ~Epoll();
    
    void detectEvent();
    void Et(int num);
    void addFd(int eventfd,bool isEt,bool isOneshot);
    void delFd(int eventfd);
    int  setNonblocking(int evnetfd);
    void  resetOneshot(int eventfd);
    void readTask(int i);
    //写任务
    void writeTask(int i,string &buf);

private:
    int epollfd;
    int listenfd;
    int log_timerfd;
    int connection_timerfd;
    int maxnum;//事件的最大数目
    epoll_event *events;    
    ThreadPoll threadpoll;
    HandleRequest handle;
    Log *log_;
    Timer timer;
};