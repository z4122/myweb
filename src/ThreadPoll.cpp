#include "ThreadPoll.hpp"


ThreadPoll::ThreadPoll(int num){
    stop = false;
    for(int i = 0;i<num;i++){
        threads.emplace_back(&ThreadPoll::WorkTread,this);
    }
    
}

ThreadPoll::~ThreadPoll(){
    stop = true;
    cv.notify_all();
    for(auto &t:threads){
        t.join();
    }
}

void ThreadPoll::addTask(Task task,int arg){
    mtx.lock();
    taskqueue.push(task);
    taskarg.push(arg);
    mtx.unlock();
    cv.notify_one();//通知一个线程执行任务
}

void ThreadPoll::WorkTread(){
    Task temptask;
    while(stop!=true){//如果stop是false，代表程序退出，析构
        int temparg = -1;
        {
            unique_lock<mutex> lk(mtx);
            cv.wait(lk,[this]{return !taskqueue.empty();});
            temptask = taskqueue.front();
            temparg = taskarg.front();
            taskqueue.pop();
            taskarg.pop();
            cout<<"thread id is:"<<std::this_thread::get_id()<<endl;
        }
        if(temparg!=-1)//貌似不用判断，会阻塞在前面的cv.wiat()
            temptask(temparg);
    }
}

