#ifndef SERVER_H
#define SERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <iostream>
#include <string>
#include <functional>

using namespace muduo;
using namespace muduo::net;
using namespace std;
class ChatServer{
public:

    // 构造函数
    ChatServer(EventLoop* loop,
               const InetAddress& listenAddr,
               const string& nameArg
               );
    // 启动
    void start();
private:
    
    // 连接回调函数
    void _connectionCallBack(const TcpConnectionPtr& conn);
    
    // 读写回调函数
    void _messageCallBack(const TcpConnectionPtr& conn,
                          Buffer* buff,
                          Timestamp time
    );

    EventLoop* _loop;
    TcpServer _server;
};
#endif