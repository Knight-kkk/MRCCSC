#include "json.hpp"
#include "public.hpp"
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include "user.hpp"
#include <vector>
#include "group.hpp"
#include <string>
#include <unordered_map>
#include <time.h>
#include <semaphore.h>

using namespace std;
using json = nlohmann::json;
bool g_logedIn = false;

void help(int fd = -1, const string& args = "");
void chat(int fd, const string& args);
void addContact(int fd, const string& args);
void createGroup(int fd, const string& args);
void joinGroup(int fd, const string& args);
void groupChat(int fd, const string& args);
void logOut(int fd = -1, const string& args = "");

// 帮助文档
unordered_map<string, string> g_commandHelpMap = {
    {"help", "[help]: 显示所有功能"},
    {"chat", "[chat:friendid:message]: 私聊消息"},
    {"addcontact", "[addcontact:friendid]: 添加好友"},
    {"creategroup", "[creategroup:name:desc]: 创建群聊"},
    {"joingroup", "[joingroup:groupid]: 加入群聊"},
    {"groupchat", "[groupchat:groupid:message]: 发送群聊消息"},
    {"logout", "[logout]: 注销登录"}
};

// 回调函数映射表
unordered_map<string, function<void(int, string)>> g_commandHandlerMap = {
    {"help", help},
    {"chat", chat},
    {"addcontact", addContact},
    {"creategroup", createGroup},
    {"joingroup", joinGroup},
    {"groupchat", groupChat},
    {"logout", logOut}
};
// 当前登录用户
User g_currentUser;

// 好友列表
vector<User> g_friends;

// 群聊列表
vector<Group> g_groups;

// 显示好友列表
void showFriends();

// 显示群聊列表
void showGroups();

// 显示自身信息
void showSelfInfo();

// 读事件处理线程
void readHandler(int clientfd);

// 当前用户信息
void showCurrentUser();

// 登陆之后主页面
void mainMenu(int clientfd);

// 格式化日期时间
string getCurrentTime();

// 读写信号量
sem_t g_rwsem;

int main(int argc, char * argv[]){
    
    if(argc < 3){
        cerr << "Usage: ./ChatClient [ServerIP] {Port]\n";
        exit(-1);
    }
    // socket()
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if(clientfd == -1){
        cerr << "socket() failed\n";
        exit(-1);
    }
    // connect()
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    if(connect(clientfd, (sockaddr*)&addr, sizeof(addr)) == -1){
        cerr << "connect() failed\n";
        close(clientfd);
        exit(-1);
    }
    // 初始化信号量
    sem_init(&g_rwsem, 0 , 0);
    // 开启读事件处理线程
    static int threadNumber = 0;
    if(threadNumber == 0){
        thread readThread(readHandler, clientfd);
        readThread.detach();   
        threadNumber++; 
    }
    cout << "***************MENU***************\n";
    cout << "[0]. Register an account\n";
    cout << "[1]. Log in\n";
    cout << "[2]. Quit\n";
    // 循环menu
    while(true){
        cout << "Input your choice:";
        string choice = "";
        getline(cin, choice);  
        if(choice == "0")
        {
            char username[20] = {0}, password[20] = {0};
            cout << "Input username:";
            cin.getline(username, 20);
            cout << "Input password:";
            cin.getline(password, 20);
            
            json js;
            js["type"] = REG_MSG;
            js["name"] = username;
            js["password"] = password;
            string request = js.dump();

            if(send(clientfd, request.c_str(), request.size(), 0) < 0){
                cerr << "send() failed when regisetr\n"; 
                break;               
            }
            sem_wait(&g_rwsem);
        }
        else if(choice == "1")
        {
            int id = -1;
            char password[20] = {0};
            cout << "Input ID:";
            cin >> id;
            cin.get();
            cout << "Input password:";
            cin.getline(password, 20);
            json js;
            js["type"] = LOGIN_MSG;
            js["id"] = id;
            js["password"] = password;
            string request = js.dump();

            int len = send(clientfd, request.c_str(), request.size(), 0);
            if(len < 0){
                cerr << "send() failed when login\n";
                break;
            }

            sem_wait(&g_rwsem);
            // 进入主界面
            if(g_logedIn){
                mainMenu(clientfd);
            }else{
                continue;
            }
        }
        else if(choice == "2")
        {
            close(clientfd);
            exit(0);
        }
        else
        {
            cerr << "invalid choice!\n";
            continue;
        }
    }
    return 0;
}


void readHandler(int clientfd){
    while(1){
        char message[1000] = {0};
        int len = recv(clientfd, message, 1000, 0);
        if(len <= 0){
            close(clientfd);
            exit(-1);
        }

        json js = json::parse(message);
        if(!js.contains("type")){
            continue ;
        }
        int type = js["type"];
        switch(type){
            case REG_MSG_ACK:
            {
                if(!js.contains("errno")){
                    continue;
                } 
                int code = js["errno"];
                if(code == 0 && js.contains("id")){
                    cout << "congratulation! succeed to register, the ID is :" << js["id"] << ", remember your ID!\n";
                    sem_post(&g_rwsem);
                    continue;
                }else{
                    cout << "fail to regiseter, reason:" << js["error"] << endl;
                    continue;
                }
                break;
            }
            case LOGIN_MSG_ACK:
            {
                if(!js.contains("errno")){
                    continue;
                }
                int code = js["errno"];
                if(code == 0){
                    g_currentUser.setID(js["id"]);
                    g_currentUser.setName(js["name"]);
                    // 解析好友信息
                    if(js.contains("friends")){
                        g_friends.clear();
                        vector<string> friendsinfo = js["friends"];
                        for(string & finfo:friendsinfo){
                            json info = json::parse(finfo);
                            g_friends.push_back(User(info["id"].get<int>(), info["name"], "", info["state"]));
                        }
                    }
                    // 解析群组信息
                    if(js.contains("groups")){
                        g_groups.clear();
                        vector<string> groupinfo = js["groups"];
                        // 解析群组信息
                        for(string& ginfo:groupinfo){
                            Group tempgroup;
                            json tempginfo = json::parse(ginfo);
                            tempgroup.setID(tempginfo["groupid"]);
                            tempgroup.setName(tempginfo["name"]);
                            tempgroup.setDesc(tempginfo["desc"]);
                            vector<string> strgroupusers = tempginfo["users"];
                            for(string& grpuserinfo:strgroupusers){
                                json tempgrpuserinfo = json::parse(grpuserinfo);
                                GroupUser tempuser;
                                tempuser.setID(tempgrpuserinfo["id"]);
                                tempuser.setName(tempgrpuserinfo["name"]);
                                tempuser.setRole(tempgrpuserinfo["role"]);
                                tempuser.setState(tempgrpuserinfo["state"]);
                                tempgroup.getUsers().push_back(tempuser);
                            }
                            g_groups.push_back(tempgroup);
                        }
                    }

                    // 显示用户基本信息
                    showCurrentUser();

                    // 解析离线消息
                    if(js.contains("offlinemsg")){
                        vector<string> strofflinemsg = js["offlinemsg"];
                        for(string& strmsg:strofflinemsg){
                            json tempjs = json::parse(strmsg);
                            if(tempjs["type"] == PRIVATE_CHAT_MSG){
                                cout << tempjs["time"] << "\t" << tempjs["name"] << "(" << tempjs["id"] << "):" << tempjs["msg"] << endl;
                                //  20090909 zhangsan(100010):hello
                            }else{
                                // 群消息
                                cout << tempjs["time"] << "\t" << tempjs["groupname"] << "(" << tempjs["groupid"] << ")'s " << tempjs["name"] << "(" << tempjs["id"] << "):" << tempjs["msg"] << endl;
                                // 20090909 test(1)'s zhsnagsan(100010): hello
                            }
                        }
                    }
                    g_logedIn = true;
                    sem_post(&g_rwsem);
                }else{
                    cout << "fail to login, reason:" << js["error"] << endl;
                    g_logedIn = false;
                    sem_post(&g_rwsem);
                    return ;
                }
                break;
            }
            case PRIVATE_CHAT_MSG:
            {
                cout << js["time"] << "\t" << js["name"] << "(" << js["id"] << "):" << js["msg"] << endl;
                //  20090909 zhangsan(100010):hello                
                break;
            }
            case GROUP_CHAT_MSG:
            {
                cout << js["time"] << "\t" << js["groupname"] << "(" << js["groupid"] << ")'s " << js["name"] << "(" << js["id"] << "):" << js["msg"] << endl;
                // 20090909 test(1)'s zhsnagsan(100010): hello            
                break;
            }
            default:
                break;
        }
    }
}


void showCurrentUser(){
    cout << "***************INFO***************\n";
    cout << "[ID]:" << g_currentUser.getID() << endl;
    cout << "[Name]:" << g_currentUser.getName() << endl;
    if(!g_friends.empty()){
        cout << "[Friens]:" << endl;    
        for(User& fri:g_friends){
            cout << "\t[ID]:" << fri.getID() << ",[Name]:" << fri.getName() << ",[State]:" << fri.getState() << endl;
        }
    }
    if(!g_groups.empty()){
        cout << "[Groups]:" << endl;
        for(Group& group:g_groups){
            cout << "\t[ID]:" << group.getID() << ",[Name]:" << group.getName() << ",[Desc]:" << group.getDesc() << endl;  
            vector<GroupUser> groupusers = group.getUsers();
            if(!groupusers.empty()){
                cout << "\t[Users]:" << endl;
                for(GroupUser& groupuser:groupusers){
                    cout << "\t\t[ID]:" << groupuser.getID() << ",[Name]:" << groupuser.getName() << ",[Role]" << groupuser.getRole() << ",[State]" << groupuser.getState()  << endl;
                }
            }            
        }
    }
    cout << "**********************************\n";
}

// 解析调用函数和功能
void mainMenu(int fd){
    help();
    while(g_logedIn){
        string command;
        getline(cin, command);
        string function;
        int pos = command.find(':');
        if(command.find(':') == -1){
            function = command;
        }else{
            function = command.substr(0, pos);
        }
        auto it = g_commandHandlerMap.find(function);
        if(it == g_commandHandlerMap.end()){
            cerr << "invalid command!\n";   
        }else{
            it -> second(fd, command.substr(pos + 1, command.size() - pos));
        }    
    }

}

void help(int fd, const string& args){
    cout << "***************MENU***************\n";
    for(auto it = g_commandHelpMap.begin() ; it != g_commandHelpMap.end(); it++){
        cout << it -> second << endl;
    }
}

void chat(int fd, const string& args){
    int pos = args.find(':');
    int id = stoi(args.substr(0, pos));
    string message = args.substr(pos + 1, args.size() - pos);
    json request;
    request["type"] = PRIVATE_CHAT_MSG;
    request["id"] = g_currentUser.getID();
    request["name"] = g_currentUser.getName();
    request["to"] = id;
    request["msg"] = message;
    // time
    request["time"] = getCurrentTime();
    string sendmsg = request.dump();
    if(send(fd, sendmsg.c_str(), sendmsg.size(), 0) == -1){
        cerr << "chat()::send() failed\n";
    }     
}

void addContact(int fd, const string& args){
    int friendid = stoi(args);
    json request;
    request["type"] = ADD_CONTACT_MSG;
    request["id"] = g_currentUser.getID();
    request["to"] = friendid;
    string sendmsg = request.dump();
    // cout << "addcontatc::message:" << sendmsg;
    if(send(fd, sendmsg.c_str(), sendmsg.size(), 0) == -1){
        cerr << "addContact()::send() failed\n";
    }
}

void createGroup(int fd, const string& args){
    int pos = args.find(':');
    string name = args.substr(0, pos);
    string desc = args.substr(pos + 1, args.size() - pos);
    json request;
    request["type"] = CREATE_GROUP_MSG;
    request["id"] = g_currentUser.getID();
    request["name"] = name;
    request["desc"] = desc;
    string sendmsg = request.dump();
    if(send(fd, sendmsg.c_str(), sendmsg.size(), 0) == -1){
        cerr << "createGroup()::send() failed\n";
    }
}

void joinGroup(int fd, const string& args){
    int groupid = stoi(args);
    json request;
    request["type"] = JOIN_GROUP_MSG;
    request["id"] = g_currentUser.getID();
    request["groupid"] = groupid;
    string sendmsg = request.dump();
    if(send(fd, sendmsg.c_str(), sendmsg.size(), 0) == -1){
        cerr << "joinGroup()::send() failed\n";
    }
}

void groupChat(int fd, const string& args){
    int pos = args.find(':');
    int groupid = stoi(args.substr(0, pos));
    string message = args.substr(pos + 1, args.size() - pos);
    json request;
    request["type"] = GROUP_CHAT_MSG;
    request["id"] = g_currentUser.getID();
    request["groupid"] = groupid;
    request["name"] = g_currentUser.getName();
    request["groupname"] = "Group";
    request["msg"] = message;
    // request["time"]
    request["time"] = getCurrentTime();
    string sendmsg = request.dump();
    if(send(fd, sendmsg.c_str(), sendmsg.size(), 0) == -1){
        cerr << "groupChat()::send() failed\n";
    }
}

void logOut(int fd, const string& args){
    json request;
    request["type"] = LOG_OUT_MSG;
    request["id"] = g_currentUser.getID();
    send(fd, request.dump().c_str(), request.dump().size(), 0);
    g_logedIn = false;
}

string getCurrentTime(){
    char buffer[101];
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo=localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return buffer;
}