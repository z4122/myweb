#pragma once
#include<thread>
#include<queue>
#include<mutex>
#include<condition_variable>
#include<functional>
#include<iostream>
using namespace std;

typedef function<void(int)> Task;

class ThreadPoll{
public:
    ThreadPoll(int num);
    ~ThreadPoll();
    //增加任务
    void addTask(Task task,int arg);
    //实际执行读写任务的线程任务
    void WorkTread();

private:
    vector<thread> threads;
    mutex mtx;
    bool stop;
    queue<Task> taskqueue;
    queue<int>  taskarg;

    condition_variable cv;
};