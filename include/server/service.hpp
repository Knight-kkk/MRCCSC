#ifndef CHATSERVICE_H
#define CHATSERVICE_H
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpConnection.h>
#include <functional>
#include <unordered_map>
#include <mutex>
#include "offlinemessagemodel.hpp"
#include "groupmodel.hpp"
#include "redis.hpp"

using namespace std;
using namespace muduo;
using namespace muduo::net;

#include "json.hpp"
#include "usermodel.hpp"
#include "friendmodel.hpp"
using json = nlohmann::json;
using handler = std::function<void(const TcpConnectionPtr&, json&, Timestamp)>;

class ChatService{

public:
    // 单例对象
    static ChatService* instance();
    // 注册函数
    void reg(const TcpConnectionPtr& conn, json &js, Timestamp time);
    // 登录函数
    void login(const TcpConnectionPtr& conn, json &js, Timestamp time);
    // 根据消息类型查询函数
    handler getHandler(int messageType);
    // 客户端异常关闭处理函数
    void clientCloseException(const TcpConnectionPtr&);
    // 私聊消息处理
    void privateChat(const TcpConnectionPtr& conn, json &js, Timestamp time);
    // 添加好友
    void addContact(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 服务端结束运行前将客户端状态重置
    void reset();
    // 创建群聊
    void createGroup(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 添加群聊
    void joinGroup(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 群聊天
    void groupChat(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 注销
    void logOut(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // redis消息通知回调函数
    void redisNotifyMessageHandler(int channle, const string& message);
private:
    ChatService();

    // 消息类型到对应处理函数的映射表
    unordered_map<int, handler> _messageHandlerMap;
    // 用户id和连接的映射表，保证线程安全的互斥锁
    unordered_map<int, TcpConnectionPtr> _userConnMap;
    mutex _userConnMapMutex;
    UserModel _usermodel;
    OfflineMsgModel _offlinemsgmodel;
    FriendModel _friendmodel;
    GroupModel _groupmodel;
    Redis _redis;
};
#endif