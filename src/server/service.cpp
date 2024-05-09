#include "public.hpp"
#include "service.hpp"
#include <iostream>
#include "user.hpp"
#include "usermodel.hpp"
#include <algorithm>
using namespace std;
using json = nlohmann::json;

ChatService* ChatService::instance(){
    static ChatService service;
    return &service;
}

// 实现注册函数
void ChatService::reg(const TcpConnectionPtr& conn, json &js, Timestamp time){
    if(!js.contains("name") || !js.contains("password")){
        return ;
    }
    cout << "新用户注册\n" << endl;
    User user;
    user.setName(js["name"]);
    user.setPassword(js["password"]);
    bool ret = _usermodel.insert(user);
    json response;
    response["type"] = REG_MSG_ACK;
    if(ret){
        response["errno"] = 0;
        response["id"] = user.getID();
    }else{
        response["errno"] = 1;
        response["error"] = "注册失败！";
    }
    conn -> send(response.dump() + '\n');    
    // 注册时有name和password字段

}


// 实现登录函数
void ChatService::login(const TcpConnectionPtr& conn, json& js, Timestamp time){
    if(!js.contains("id") || !js.contains("password")){
        return ;
    }
    cout << "用户登录\n" << endl;
    int userid = js["id"].get<int>();
    User user = _usermodel.query(userid);
    if(user.getID() == -1){
        // 未查询到该用户或者查询错误
        json response;
        response["type"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["id"] = userid;
        response["error"] = "用户不存在或查询错误";
        conn -> send(response.dump() + '\n');
    }else{
        // 检查密码是否正确
        if(js["password"] == user.getPassword()){
            if(user.getState() == "online"){
                json response;
                response["type"] = LOGIN_MSG_ACK;
                response["errno"] = 1;
                response["error"] = "用户已在线，重复登录！";
                response["id"] = userid;   
                conn -> send(response.dump() + '\n');            
            }else{
                json response;
                response["type"] = LOGIN_MSG_ACK;
                response["errno"] = 0;
                response["id"] = userid;
                response["name"] = user.getName();
                // 推送离线消息
                response["offlinemsg"] = _offlinemsgmodel.query(user.getID()); 
                // 加载好友列表
                vector<User> friends = _friendmodel.query(userid);     
                vector<string> strfriends;
                for(User& user:friends){
                    json tempjs;
                    tempjs["id"] = user.getID();
                    tempjs["name"] = user.getName();
                    tempjs["state"] = user.getState();
                    strfriends.push_back(tempjs.dump());
                }  
                vector<Group> groups = _groupmodel.queryGroups(userid);
                vector<string> strgroups;
                for(Group group:groups){
                    json tempjs;
                    tempjs["groupid"] = group.getID();
                    tempjs["name"] = group.getName();
                    tempjs["desc"] = group.getDesc();
                    vector<GroupUser> grpusers = group.getUsers();
                    vector<string> strgrpusers;
                    for(GroupUser& grpuser:grpusers){
                        json ttempjs;
                        ttempjs["id"] = grpuser.getID();
                        ttempjs["name"] = grpuser.getName();
                        ttempjs["state"] = grpuser.getState();
                        ttempjs["role"] = grpuser.getRole();
                        strgrpusers.push_back(ttempjs.dump());
                    }
                    tempjs["users"] = strgrpusers;
                    strgroups.push_back(tempjs.dump());
                } 
                response["friends"] = strfriends;
                response["groups"] = strgroups;
                cout << __LINE__ << response.dump() << endl;
                conn -> send(response.dump() + '\n');
                _offlinemsgmodel.remove(user.getID());
                // 更新状态为online
                _usermodel.updateState(userid, "online");   
                // 将用户id和tcp连接加入map
                // 花括号构成作用于，保证锁的粒度足够小
                {
                    lock_guard<mutex> lock(_userConnMapMutex);
                    _userConnMap.insert({userid, conn});
                } 
                // 订阅redis消息队列
                _redis.subscribe(userid);        
            }
        }else{
            json response;
            response["type"] = LOGIN_MSG_ACK;
            response["errno"] = 1;
            response["error"] = "密码错误";
            response["id"] = userid;
            conn -> send(response.dump() + '\n');
        }      
    }
}

// 回调函数查询函数
handler ChatService::getHandler(int messageType){
    auto it = _messageHandlerMap.find(messageType);
    if(it == _messageHandlerMap.end()){
        std::cout << "There is no corresponding function for type:" << messageType << endl;
        return [](const TcpConnectionPtr&, json&, Timestamp){}; 
    }
    return it -> second;
}

// 客户端异常关闭处理函数
void ChatService::clientCloseException(const TcpConnectionPtr& conn){
    int userid = -1;
    // 根据conn查找map，找到对应的userid
    {
        lock_guard<mutex> lock(_userConnMapMutex);
        for(auto it = _userConnMap.begin(); it != _userConnMap.end();it++){
            if(it -> second == conn){
                userid = it -> first;
                _userConnMap.erase(userid);
                // 取消订阅
                _redis.unsubscribe(userid);
            }
        }
    }
    // 根据id更新状态为offline
    if(userid != -1){
        _usermodel.updateState(userid, "offline");
    }
}

// 私聊消息处理
void ChatService::privateChat(const TcpConnectionPtr& conn, json &js, Timestamp time){
    if(js.contains("to")){
        int toid = js["to"];
        {
            lock_guard<mutex> lock(_userConnMapMutex);
            auto it = _userConnMap.find(toid);
            if(it != _userConnMap.end()){
                // 接收方与发送方在同一服务器上，且在线
                it -> second -> send(js.dump());
            }else{
                // 接收方和发送方不在同一服务器或者接收方不在线
                User user = _usermodel.query(toid);
                if(user.getState() == "offline"){
                    _offlinemsgmodel.insert(toid, js.dump());
                }else{
                    _redis.publish(toid, js.dump());
                }
            }
        }   
    }

}
// 重置在线状态
void ChatService::reset(){
    _usermodel.reset();
}
// 添加好友
void ChatService::addContact(const TcpConnectionPtr& conn, json &js, Timestamp time){
    if(!js.contains("id") || !js.contains("to")){
        return ;
    }
    _friendmodel.insert(js["id"], js["to"]);
}

// 建群
void ChatService::createGroup(const TcpConnectionPtr& conn, json &js, Timestamp time){
    if(!js.contains("id") || !js.contains("name") || !js.contains("desc")){
        return ;
    }
    // 创建群
    Group group(-1, js["name"], js["desc"]);
    _groupmodel.create(group);
    // 将此人设为群主
    _groupmodel.join(js["id"], group.getID(), "admin");
}

// 加群
void ChatService::joinGroup(const TcpConnectionPtr& conn, json &js, Timestamp time){
    if(!js.contains("id") || !js.contains("groupid")){
        return ;
    }
    _groupmodel.join(js["id"], js["groupid"]);
}

// 群聊
void ChatService::groupChat(const TcpConnectionPtr& conn, json &js, Timestamp time){
    if(!js.contains("id") || !js.contains("groupid")){
        return ;
    }
    vector<int> users = _groupmodel.queryGroupUsers(js["id"], js["groupid"]);
    {
        lock_guard<mutex> lock(_userConnMapMutex);
        for(int userid:users){
            auto it = _userConnMap.find(userid);
            if(it != _userConnMap.end()){
                // 在同一个服务器上且在线
                it -> second -> send(js.dump());
            }
            else{
                // 不在线或者不在同一个服务器上
                User user = _usermodel.query(userid);
                if(user.getState() == "offline"){
                    _offlinemsgmodel.insert(userid, js.dump());
                }else{
                    _redis.publish(userid, js.dump());
                }
            }
        }
    }
}

// 注销
void ChatService::logOut(const TcpConnectionPtr& conn, json& js, Timestamp time){
    lock_guard<mutex> lock(_userConnMapMutex);
    auto it = _userConnMap.find(js["id"]);
    if(it != _userConnMap.end()){
        _userConnMap.erase(js["id"]);
        _usermodel.updateState(js["id"], "offline");
        // 取消redis订阅
        _redis.unsubscribe(js["id"]);
    }
}
// 初始化map
ChatService::ChatService(){
    cout << "ChatService构造函数\n";
    _messageHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    _messageHandlerMap.insert({REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
    _messageHandlerMap.insert({PRIVATE_CHAT_MSG, std::bind(&ChatService::privateChat, this, _1, _2, _3)});
    _messageHandlerMap.insert({ADD_CONTACT_MSG, std::bind(&ChatService::addContact, this, _1, _2, _3)});
    _messageHandlerMap.insert({CREATE_GROUP_MSG, std::bind(&ChatService::createGroup, this , _1, _2 , _3)});
    _messageHandlerMap.insert({JOIN_GROUP_MSG, std::bind(&ChatService::joinGroup, this, _1, _2, _3)});
    _messageHandlerMap.insert({GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3)});
    _messageHandlerMap.insert({LOG_OUT_MSG, std::bind(&ChatService::logOut, this , _1, _2, _3)});

    // 连接redis
    if(_redis.connect()){
        cout << "ChatService::ChatService()::redis.connect() succeed!\n";
    }else{
        cout << "fail to conenct redis\n";
    }  
    // 设置redis消息通知回调函数
    _redis.setNotifyMessageHandler(std::bind(&ChatService::redisNotifyMessageHandler, this, _1, _2));  
}

void ChatService::redisNotifyMessageHandler(int channel, const string& message){
    // 考虑两种情况
    // 第一, channel订阅client在线，将message发送给client
    // 第二，channel订阅client不在线，将message存储进离线消息
    lock_guard<mutex> lock(_userConnMapMutex);
    if(_userConnMap.find(channel) != _userConnMap.end()){
        _userConnMap.find(channel) -> second -> send(message);
    }else{
        _offlinemsgmodel.insert(channel, message);
    }
}



