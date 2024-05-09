// 群组增删改查类
#ifndef GROUPMODEL_H
#define GROUPMODEL_H
#include <string>
#include "db.hpp"
#include "group.hpp"
#include "groupuser.hpp"

using namespace std;
class GroupModel{
public:
    bool create(Group& group);
    bool join(int userid, int groupid, const string& role =  "normal");
    vector<Group> queryGroups(int userid);
    vector<int> queryGroupUsers(int userid, int groupid);
private:

};
#endif