#include "server.hpp"
using namespace std;
#include <signal.h>
#include "service.hpp"

void signalHandler(int sig){
    if(sig == SIGINT || sig == SIGSEGV){
        ChatService::instance() -> reset();
        exit(0);
    }
}

int main(int argc, char** argv){
    if(argc < 3){
        cerr << "Usage:./ChatServer [IP] [Port]\n";
        return -1;
    }
    cout << "server::main函数";
    EventLoop loop;
    InetAddress addr(argv[1], atoi(argv[2]));
    ChatServer server(&loop, addr, "test");
    signal(SIGINT, signalHandler);
    signal(SIGSEGV, signalHandler);
    server.start();
    loop.loop();
    return 0;
}