/*
muduo网络库使用流程
1. 在自己的服务器类中组合eventloop和tcpserver
2. 重写构造函数和析构函数
3. 定义连接事件回调函数和读写事件回调函数
4. 对server设置回调函数
5. main中定义server，启动监听和事件循环，设置核数

编译链接顺序：-lmuduo_net -lmuduo_base -lpthread
*/

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <iostream>
#include <string>
#include <functional>

using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;

class ChatServer{
public:
    
    ChatServer(EventLoop* loop, 
                const InetAddress& listenaddr, 
                const string& nameArg):
        _server(loop, listenaddr, nameArg), _loop(loop)
    {
        cout << "Constructor of Class CahtServer.\n";


        // 设置连接回调函数
        _server.setConnectionCallback(std::bind(&ChatServer::_connectionCallBack, this, _1));
        // 设置读写事件回调函数
        _server.setMessageCallback(std::bind(&ChatServer::_messageCallBack, this, _1, _2, _3));
    }

    ~ChatServer(){}

    void start(){
        _server.start();
    }
private:
    TcpServer   _server;    // muduo server
    EventLoop   *_loop;     // event loop

    void _connectionCallBack(const TcpConnectionPtr& conn){
        if(conn -> connected()){
            cout << "New Connection from: " << conn -> peerAddress().toIpPort() << endl;
        }
        else{
            cout << "Close Connection from:" << conn -> peerAddress().toIpPort() << endl;
            conn -> shutdown();
        }
    }

    void _messageCallBack(const TcpConnectionPtr& conn, Buffer* buff, Timestamp time){
        string message = buff -> retrieveAllAsString();
        cout << time.toString() << "  receive message: " << message << ", from: " << conn -> peerAddress().toIpPort() << endl;
        conn -> send(message);
    }
};


int main(){
    
    EventLoop loop;
    InetAddress addr("127.0.0.1", 6666);
    string servername = "test";
    ChatServer server(&loop, addr, servername);

    server.start();     // 相当于将listenfd加入epoll
    loop.loop();        // 相当于epoll_wait()


    return 0;
}
