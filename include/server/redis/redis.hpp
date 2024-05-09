#ifndef REDIS_H
#define REDIS_H
#include <hiredis/hiredis.h>
#include <thread>
#include <functional>
using namespace std;

class Redis{
public:
    Redis();
    ~Redis();

    void setNotifyMessageHandler(function<void(int, const string&)> handler);
    bool connect();
    bool publish(int chanel, const string& messages);
    bool subscribe(int channel);
    bool unsubscribe(int channel);
    // 监听消息
    void receiveMessage(); 
private:
    // publish context
    redisContext* _publishContext;
    // subscribe context
    redisContext* _subscribeContext;
    // notify handler   _nofityMessageHanlder(channel id, message)
    function<void(int, const string&)> _notifyMessageHandler;
};
#endif