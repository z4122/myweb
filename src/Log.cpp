#include "Log.hpp"


Log::Log(string name):thread_(bind(&Log::writeThread,this)),
        filename(name),
        file(name,std::ios::app)
{
    currentbuf = make_shared<string>();
    currentbuf->reserve(2000);
    nextbuf = make_shared<string>();
    nextbuf->reserve(2000);
}

Log::~Log(){
    file.close();
}

void Log::startWrite(){
    mtx.lock();

    buffers.emplace_back(currentbuf);
    if(nextbuf!=NULL){
        currentbuf = nextbuf;
    }        
    else{
        currentbuf = make_shared<string>();
        currentbuf->reserve(2000);
    }
    cv.notify_all();

    mtx.unlock();
}

void Log::writeThread(){
    shared_ptr<string> buf1(make_shared<string>());
    shared_ptr<string> buf2(make_shared<string>());
    buf1->reserve(2000);
    buf2->reserve(2000);
    while(run){
        shared_ptr<string> tempbuf;
        {
            unique_lock<mutex> lk(mtx);
            if(buffers.empty()){
                cv.wait(lk);
            }
            currentbuf = move(buf1);
            buffersToWrite.swap(buffers);
            if(nextbuf==NULL)
                nextbuf = move(buf2);
        }
        
        //向文件中写
        for(auto iter = buffersToWrite.begin();iter!=buffersToWrite.end();iter++)
        {
            cout<<**iter<<endl;
            file<<**iter<<endl;
        }

       if(buf1.use_count()==false)
        {
            buf1 = move(buffersToWrite.back());
            buffersToWrite.pop_back();
            buf1->clear();
        }

        if(buf2.use_count()==false){
            buf2 = move(buffersToWrite.back());
            buffersToWrite.pop_back();
            buf2->clear();
        }

        buffersToWrite.clear();

    }
}

//还有优化的空间，用stringbuf感觉有点慢
string Log::getLocalTime(){
    stringbuf sbuf;
    ostream out(&sbuf);
    struct tm *t;
    time_t tt;
    time(&tt);
    t = localtime(&tt);
    out<<t->tm_year+1900<<"年"<<t->tm_mon+1<<"月"<<t->tm_mday<<"日"<<t->tm_hour<<"时";
    out<<t->tm_min<<"分"<<t->tm_sec<<"秒";
    string temp =  move(sbuf.str());
    return temp;
}

void Log::tagTime(){
    mtx.lock();
    stringbuf sbuf;
    ostream out(&sbuf);
    out<<getLocalTime()+"\t";
    *currentbuf+=sbuf.str();
    mtx.unlock();
}


