#pragma once
#include<iostream>
#include<regex>
#include<string>
#include<vector>
#include<fstream>
#include<unordered_map>

using namespace std;

typedef struct Request{
    //请求的http版本，路径，类型
    string version,path,type;
    //请求全部内容
    string content;
    //解析出来的头部及其内容
    unordered_map<string,string> header;
    //正则表达式
    smatch match;
}Request;

//封装，第一个string代表类型type，第二个string代表正则表达式
typedef map<string,unordered_map<string,function<void(ostream&,Request&)>>> resource_type;

class HandleRequest{
public:
    HandleRequest();
    void addResource();
    void Respond(ostream &response,Request &request);
    Request Prase(string &content);

private:
    resource_type resource;
    resource_type default_resource;
};