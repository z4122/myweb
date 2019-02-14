#pragma once
#include<thread>
#include<string>
#include<condition_variable>
#include<memory>
#include<vector>
#include<iostream>
#include<sstream>
#include<fstream>
#include"Nocopyable.hpp"
using namespace std;

class Log:public Nocopyable{
public:
    Log(string name);
    ~Log();
    void writeThread();
    void startWrite();
    string getLocalTime();
    void tagTime();
    template<typename T>
    Log& operator<<(T input){
        mtx.lock();
        stringbuf sbuf;
        ostream out(&sbuf);
        out<<input;
        *currentbuf+=sbuf.str();
        mtx.unlock();
        //如果存储的信息够大，则更换buf区域
        if(currentbuf->size()>1000){
            startWrite();
        }      
        return *this;
    }

private:
    condition_variable cv;
    mutex mtx;
    vector<shared_ptr<string>> buffers,buffersToWrite;
    shared_ptr<string> currentbuf,nextbuf;
    thread thread_;
    bool run = true;
    ofstream file;
	string filename;

};