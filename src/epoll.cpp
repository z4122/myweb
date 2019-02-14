#include"epoll.hpp"

#define BUFFER_SIZE 1024


Epoll::Epoll(int num,Log *log):
        events(new epoll_event[num]),
        maxnum(num),
        threadpoll(10),
        log_(log)
{
    int port = 8080;
    struct sockaddr_in address;
    int ret;
    
    bzero(&address,sizeof(address));
    address.sin_addr.s_addr = htons(INADDR_ANY);
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    
    listenfd = socket(AF_INET,SOCK_STREAM,0);//TCP
    assert(listenfd>=0);

    //设置端口可立刻重复连接，在实际使用中，要注释掉。
    int on = 1;
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

    ret = bind(listenfd,(struct sockaddr*)&address,sizeof(address));
    assert(ret!=-1);

    ret = listen(listenfd,num);//监听的最大数目
    assert(ret!=-1);

    epollfd = epoll_create(num);
    assert(epollfd!=-1);

    addFd(listenfd,true);//增加监听事件,true代表开启ET模式，false代表LT模式

    log_timerfd = timer.addTimerEvery(3,0);
    connection_timerfd = timer.addTimerAfter(300000,0);

    addFd(log_timerfd,true);
    addFd(connection_timerfd,true);

}

Epoll::~Epoll(){
    close(epollfd);
    close(listenfd);
}

//设置文件描述符为非阻塞
int Epoll::setNonblocking(int eventfd){
    int old_option = fcntl(eventfd,F_GETFL);
    int new_option = old_option|O_NONBLOCK;

    fcntl(eventfd,F_SETFL,new_option);

    return old_option;

}

void Epoll::addFd(int eventfd,bool isEt){
    epoll_event event;
    event.data.fd = eventfd;
    event.events = EPOLLIN;
    if(isEt){
        event.events|=EPOLLET;
    }
    epoll_ctl(epollfd,EPOLL_CTL_ADD,eventfd,&event);
}

void Epoll::delFd(int eventfd){
    epoll_ctl(epollfd,EPOLL_CTL_DEL,eventfd,0);
}

void Epoll::detectEvent(){
    int ret = epoll_wait(epollfd,events,maxnum,-1);
    assert(ret>=0);
    Et(ret);//Et触发
}

void Epoll::Et(int eventnum){

    for(int i = 0;i<eventnum;i++){
        if(events[i].data.fd==listenfd)//来到的事件是监听事件
        {
            struct sockaddr_in client_address;
            socklen_t client_address_length = sizeof(client_address);
            int connfd = accept(listenfd,(struct sockaddr*)&client_address,&client_address_length);
            cout<<"add a connection event:"<<connfd<<endl;
            addFd(connfd,true);//加入事件到epollfd中，接着监听
        }
        else if((events[i].data.fd == log_timerfd))//3s周期定时器事件
        {
            uint64_t exp;
            //必须有下面这句话，清空缓存，才能够继续触发log_timerfd
            read(log_timerfd,&exp,sizeof(uint64_t));
            log_->startWrite();
            cout<<"3s定时器触发一次"<<endl;
            
        }
        else if(events[i].data.fd == connection_timerfd)//定时器堆事件
        {
            //7s之后关闭连接
            pair<struct timeval,int> temp = timer.topHeapValue();
            while(temp.first.tv_sec>=timer.topHeapValue().first.tv_sec)
            {
                //close(timer.topHeapValue().second);
                //delFd(timer.topHeapValue().second);
                cout<<"7s关闭连接"<<timer.topHeapValue().second<<endl;
                timer.popHeapValue();
                if(timer.isHeapEmpty())
                    break;
            }
            uint64_t exp;
            //必须有下面这句话，清空缓存，才能够继续触发log_timerfd
            //read(connection_timerfd,&exp,sizeof(uint64_t));
            if(!timer.isHeapEmpty())
                timer.changeTimer(connection_timerfd,timer.topHeapValue().first.tv_sec,0,0,0); 
        }
        else if(events[i].events&EPOLLIN)//来到的事件是读事件
        {
            printf("receive once\n");
            if(timer.isHeapEmpty())
                timer.changeTimer(connection_timerfd,7,0,0,0);//7秒后触发一次定时中端。
            timer.addHeapValue(events[i].data.fd);
            threadpoll.addTask(bind(&Epoll::readTask,this,placeholders::_1),events[i].data.fd);

        }
        else
        {
            printf("Something else happens\n");
        }
    }
}

void Epoll::readTask(int fd){
    char buf[BUFFER_SIZE];
    while(1){
        int ret = recv(fd,buf,BUFFER_SIZE-1,0);
        if(ret<0){
        //条件成立对于非阻塞IO来说，代表着数据已经读取完毕，下一次可以再次监听到EPOLLIN
            if(errno==EAGAIN||errno==EWOULDBLOCK){
                printf("read later\n");
                break;
            }
        }
        else if(ret==0){//接受完毕数据
            cout<<"关闭了fd:"<<fd<<endl;
            log_->tagTime();
            *log_<<"关闭了fd:"<<fd<<'\n';
            close(fd);
            delFd(fd);
            break;
        }
        else{//正常接收到ret个字节
            //printf("received %d byte of %s\n",ret,buf);
            string temp=buf;//这里应该能改成stringbuf
            //cout<<"收到的字符串为："<<endl;
            //cout<<temp;
            //cout<<"收到的字符串输出完毕"<<endl;
            Request request = handle.Prase(temp);
            stringbuf sbuf;
            std::ostream response(&sbuf);
            handle.Respond(response,request);
            //cout<<"输出的内容为:"<<endl;
            temp = sbuf.str();
            //cout<<temp<<endl;
            //cout<<"输出的内容输出完毕"<<endl;
            writeTask(fd,temp);
           
        }
    }
}


void Epoll::writeTask(int fd,string &buf){
    const char *content = buf.c_str();
    log_->tagTime();
    *log_<<"fd为:"<<fd<<'\n';
    int ret = write(fd,content,buf.size()+1);
    if(ret==-1){
        cout<<"发送失败"<<endl;
    }
    else{
        cout<<"发送成功,发送字节数为："<<ret<<endl;
    }
    
}