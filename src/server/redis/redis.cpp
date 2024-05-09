#include "redis.hpp"
#include <hiredis/hiredis.h>
#include <iostream>
#include <functional>
using namespace std;
Redis::Redis(): _publishContext(nullptr), _subscribeContext(nullptr){
    
}

Redis::~Redis(){
    if(_publishContext){
        redisFree(_publishContext);
    }
    if(_subscribeContext){
        redisFree(_subscribeContext);
    }
}

bool Redis::connect(){
    cout << "Redis::connect()\n";
    _publishContext = redisConnect("127.0.0.1", 6379);
    if(!_publishContext){
        cerr << "Redis::connect() failed\n";
        return false;
    }

    _subscribeContext = redisConnect("127.0.0.1", 6379);
    if(!_subscribeContext){
        cerr << "Redis::connect() failed\n";
        return false;
    }
    // 开始监听
    thread t([&](){receiveMessage();});
    t.detach();
    return true;
}

bool Redis::publish(int channel, const string& message){
    redisReply* reply = (redisReply*)redisCommand(_publishContext, "PUBLISH %d %s", channel, message.c_str());
    if(!reply){
        cerr << "Redis::publish() failed\n";
        return false;
    }
    freeReplyObject(reply);
    return true;
}

// 
bool Redis::subscribe(int channel){
    // redisCommand(_subscribeContext, "SUBSCRIBE %D", channel);
    // 不能使用rediscommand函数，因为rediscommand = appendcommand + bufferwrite + getreply(会阻塞等待)
    // 这样会阻塞等待reply
    if(REDIS_ERR == redisAppendCommand(_subscribeContext, "SUBSCRIBE %d", channel)){
        cerr << "Redis::subscribe()::redisAppendCommand() failed\n";
        return false;
    }

    int done = 0;
    while(!done){
        if(REDIS_ERR == redisBufferWrite(_subscribeContext, &done)){
            cerr << "Redis::subscribe()::redisBufferWrite() failed\n";
            return false;
        }
    }

    return true;
}

bool Redis::unsubscribe(int channel){
    if(REDIS_ERR == redisAppendCommand(_subscribeContext, "UNSUBSCRIBE %d", channel)){
        cerr << "Redis::unsubscribe()::redisAppendCommand() failed\n";
        return false;
    }
    
    int done = 0;
    while(!done){
        if(REDIS_ERR == redisBufferWrite(_subscribeContext, &done)){
            cerr << "Redis::unsubscribe()::redisBufferWrite() failed\n";
            return false;
        }    
    }

    return true;
}

void Redis::receiveMessage(){
    redisReply* reply = nullptr;
    while(REDIS_OK == redisGetReply(_subscribeContext, (void**)(&reply))){
        if(reply && reply -> element[1] && reply -> element[1] -> str && reply -> element[2] && reply -> element[2] -> str){
            cout << "redis::receive()\n";
            _notifyMessageHandler(atoi(reply -> element[1] -> str), reply -> element[2] -> str);
        } 

        freeReplyObject(reply);
    }
    cerr << "Redis::receiveMessage()::redisGetReply() failed\n";
}

void Redis::setNotifyMessageHandler(function<void(int, const string&)> handler){
    _notifyMessageHandler = handler;
}