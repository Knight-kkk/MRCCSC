#include "redis.hpp"
#include <string>
#include <iostream>
#include <hiredis/hiredis.h>
using namespace std;
int main(){
    Redis redis;
    redis.connect();
    redis.setNotifyMessageHandler([](int channel, const string& message){cout << message.c_str();});
    redis.subscribe(1);
    while(1){
        cout << "输入想要发布的消息:";
        string message;
        getline(cin,message);
        redis.publish(2, "hello, this is a test demo.");    
    }
    return 0;
}
