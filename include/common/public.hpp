#ifndef PUBLIC_H
#define PUBLIC_H


// server和client公共头文件
enum{
    LOGIN_MSG = 1,
    LOGIN_MSG_ACK,
    REG_MSG,
    REG_MSG_ACK,
    PRIVATE_CHAT_MSG,
    ADD_CONTACT_MSG,
    CREATE_GROUP_MSG,
    JOIN_GROUP_MSG,
    GROUP_CHAT_MSG,
    LOG_OUT_MSG,
};
#endif




// {"type":1,"id":2,"password":"123456789"}
// {"type":5,"to":2,"msg":"hello,zhangsan!"}