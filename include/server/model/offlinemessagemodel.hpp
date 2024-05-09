#ifndef OFFLINEMSG_H
#define OFFLINEMSG_H
#include <string>
#include <vector>

using namespace std;

class OfflineMsgModel{
public:
// 增加离线消息
    bool insert(int, const string&);

    // 查询离线消息
    vector<string> query(int);

    // 删除离线消息
    bool remove(int);
private:

};
#endif