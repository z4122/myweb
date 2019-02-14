#include "HandleRequest.hpp"

HandleRequest::HandleRequest(){
    addResource();
}

void HandleRequest::addResource(){
    resource["POST"]["^/string/?$"] = [](ostream &response,Request &request){
        response<<"HTTP/1.1 200 OK\r\nContent-Length: "<<request.content.length()<<"\r\n\r\n"<<request.content;
    };
    
    resource["GET"]["^/info/?$"] = [](ostream &response,Request &request){
        stringstream content_stream;
        content_stream<<"<h1>Request:</h1>";
        content_stream<<request.type<<" "<<request.path<<"HTTP/"<<request.version<<"<br>";
        for(auto &header:request.header){
            content_stream<<header.first<<":"<<header.second<<"<br>";
        }

        content_stream.seekp(0,ios::end);//把指针指向流的最后，下面tellp（）是计算了当前的长度，通过这种方法就能够计算流的长度

        response<<"HTTP/1.1 200 OK\r\nContent-Length:"<<content_stream.tellp()<<"\r\n\r\n"<<content_stream.rdbuf();
    };

    resource["GET"]["^/match/([0-9a-zA-Z]+)/?$"] = [](ostream &response,Request &request){
        string number = request.match[1];//匹配结果的第一项就是([0-9a-zA-Z]+)
        response<<"HTTP/1.1 200 OK\r\nContent-Length:"<<number.length()<<"\r\n\r\n"<<number;
    };

    resource["POST"]["^.*$"] = [&](ostream &response,Request &request){
        string name = request.header["Username"];
        string password = request.header["Password"];
        cout<<"Post item Username:"<<request.header["Username"]<<"\tPassword:"<<request.header["Password"]<<endl;
        //if(server.db->GetValue(name)==password){
            response<<"HTTP/1.1 303 See Other\r\nLocation:"<<"http://localhost:8080/index.html"<<"\r\n\r\n";
        //}
        //else{
        //    response<<"HTTP/1.1 303 See Other\r\nLocation:"<<"http://localhost:8080/register.html"<<"\r\n\r\n";
        //}
    };

    //匹配提交的用户名和密码                     
    resource["GET"]["^.*?registerUsername=(.*)&registerEmail=(.*)&registerPassword=(.*)&registerAgree=1&registerSubmit=$"] = [&](ostream &response,Request &request){
        //server.db->SetValue(request.match[1],request.match[3]);
        //cout<<"register item\t"<<"name:"<<request.match[1]<<"\tpassword:"<<server.db->GetValue(request.path_match[1])<<endl;
        
        /*
        //之后自动跳转到login登录界面
        string filename = "web/login.html";
        ifstream ifs;
        ifs.open(filename,ifstream::in);

        if(ifs){
            ifs.seekg(0,ios::end);
            int length = ifs.tellg();
            ifs.seekg(0,ios::beg);

            cout<<filename<<"\tlength:"<<length<<endl;
            response<<"HTTP/1.1 200 OK\r\nContent-length:"<<length<<"\r\n\r\n"<<ifs.rdbuf();
        }
        else{
            string content = "Could not open file";
            response<<"HTTP/1.1 400 Bad Request\r\nContent-Length:"<<content.length()<<"\r\n\r\n"<<content;
        }
        */
        
    };
    
    resource["GET"]["^/?(.*)\.(.{2,4})$"] = [](ostream &response,Request &request){
        string filename = "web";
        string filepath = request.match[0];

        size_t last_pos = filepath.rfind("?v=");
        if(last_pos!=string::npos)
            filepath.erase(last_pos,8);
        
        last_pos = filepath.rfind(".map");
        if(last_pos!=string::npos)
            filepath.erase(last_pos,4);

        filename = filename+filepath;
        cout<<"source item    "<<filename<<endl;
        
        ifstream ifs;
        ifs.open(filename,ifstream::in);
        cout<<"normal:"<<filename<<endl;

        if(ifs){
            ifs.seekg(0,ios::end);
            int length = ifs.tellg();
            ifs.seekg(0,ios::beg);
            cout<<filename<<"   length:"<<length<<endl;
            response<<"HTTP/1.1 200 OK\r\nContent-Length:"<<length<<"\r\n\r\n"<<ifs.rdbuf();
        }
        else{
            string content = "Could not open file";
            cout<<content<<endl;
            response<<"HTTP/1.1 400 Bad Request\r\nContent-Length:"<<content.length()<<"\r\n\r\n"<<content;
        }

    };
    
    //最后一个是默认的资源。
    //匹配0个或1个/，再匹配任意个字符，相当于是匹配其他所有的请求
    default_resource["GET"]["^/?(.*)$"] = [](ostream &response,Request &request){
        string filename = "web/";
        string path = request.match[1];

        cout<<path<<endl;
        size_t last_pos = path.rfind(".");//从结尾找.
        size_t current_pos = 0;
        size_t pos;
        
        while((pos=path.find('.',current_pos))!=string::npos&&pos!=last_pos){//从开头找.。!=npos代表能够从字符串中找到.，相当于是为了滤除..进入上一个目录
            current_pos = pos;
            path.erase(pos,1);
            last_pos--;//last_pos--,仍然指向的是最后一个.，因为前面erase了，所以长度自动减去1
        }

        filename += path;
        //针对路径中没有.的请求，加上index.html进行查看，如果有.就不进行这一步，仅仅是保留了最后一个.
        if(filename.find('.')==string::npos){
            if(filename[filename.length()-1]!='/')
                filename += '/';
            filename += "index.html";
        }

        ifstream ifs;
        ifs.open(filename,ifstream::in);
        cout<<"default:"<<filename<<endl;

        if(ifs){
            ifs.seekg(0,ios::end);
            int length = ifs.tellg();
            ifs.seekg(0,ios::beg);
            response<<"HTTP/1.1 200 OK\r\nContent-Length:"<<length<<"\r\n\r\n"<<ifs.rdbuf();
        }//如果打开
        else{
            string content = "Counld not open file"+filename;
            cout<<content<<endl;
            response<<"HTTP/1.1 400 Bad Request\r\nContent-Length:"<<content.length()<<"\r\n\r\n"<<content;
        }
    };

    
    

}

Request HandleRequest::Prase(string &content){
    Request request;
    string line;
    stringstream ss(content);
    regex e("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    smatch submatch;
    getline(ss,line);
    line.pop_back();//去除后面的换行符
    if(regex_match(line,submatch,e)){
        request.type = submatch[1];
        request.path = submatch[2];
        request.version = submatch[3];

        request.content=content;

        bool matched = false;
        do{
            getline(ss,line);
            line.pop_back();
            e = "^([^:]*): ?(.*)";
            matched = regex_match(line,submatch,e);
            if(matched == true)
                request.header[submatch[1]] = submatch[2];
        }while(matched);

        if(request.type=="POST"){
			getline(ss,line);
			//line.pop_back();
			e="^loginUsername=(.*)&loginPassword=(.*)&loginSubmit=$";
			matched=regex_match(line,submatch,e);
			if(matched){
				request.header["Username"] = submatch[1];
				request.header["Password"] = submatch[2];
				}
			}
    }

    return request;
}

void HandleRequest::Respond(ostream &response,Request &request){
    //首先找请求的类型
    if(resource.find(request.type)!=resource.end())
    {
        //逐个判断正则表达式
        for(auto re:resource[request.type]){
            regex e(re.first);
            cout<<"正则表达式为:"<<re.first<<endl;
            smatch match;
            if(regex_match(request.path,match,e)){
                request.match=move(match);
                re.second(response,request);
                return;
            }
        }
    }

    if(default_resource.find(request.type)!=default_resource.end())
    {
        for(auto re:default_resource[request.type]){
            regex e(re.first);
            cout<<"正则表达式为:"<<re.first<<endl;
            smatch match;
            if(regex_match(request.path,match,e)){
                request.match=move(match);
                re.second(response,request);
                return;
            }
        }
    }

    
}

