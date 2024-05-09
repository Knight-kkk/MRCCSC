#include "server.hpp"
using namespace std;
using namespace placeholders;
#include "json.hpp"
#include "service.hpp"
using json = nlohmann::json;
// 实现构造函数
ChatServer::ChatServer( EventLoop* loop,
                        const InetAddress& listenAddr,
                        const string& nameArg): _loop(loop), _server(loop, listenAddr, nameArg)
{
    cout << "server构造函数\n";
    // 设置回调函数
    _server.setConnectionCallback(std::bind(&ChatServer::_connectionCallBack, this, _1));
    _server.setMessageCallback(std::bind(&ChatServer::_messageCallBack, this, _1, _2, _3));

    // 设置CPU核数
    _server.setThreadNum(8);
}

// 启动
void ChatServer::start(){
    _server.start();
}


    
// 连接回调函数
void ChatServer::_connectionCallBack(const TcpConnectionPtr& conn){
    if(conn -> connected()){
        std::cout << "Client: " << conn -> peerAddress().toIpPort() << " is online." << std::endl;
    }else{ 
        std::cout << "Client: " << conn -> peerAddress().toIpPort() << " is offline." << std::endl;
        conn -> shutdown();
        ChatService::instance() -> clientCloseException(conn);
    }
}
    
// 读写回调函数
void ChatServer::_messageCallBack(const TcpConnectionPtr& conn,
                        Buffer* buff,
                        Timestamp time
){
    string message = buff -> retrieveAllAsString();
    cout << "server.cpp::message:" << message << endl;
    json js = json::parse(message);
    if(!js.contains("type")){
        return ;
    }
    handler func = ChatService::instance() -> getHandler(js["type"]);
    func(conn, js, time);
    std::cout <<  time.toFormattedString() << " , receive message:" << message.data() << " from: " << conn -> peerAddress().toIpPort() << std::endl;
}

