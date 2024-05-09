// 群组类
#ifndef GROUP_H
#define GROUP_H
#include <string>
#include <vector>
#include "groupuser.hpp"
using namespace std;

class Group{
public:
    Group(int id = -1, const string& name = "", const string& desc = ""): id(id), name(name), desc(desc){ }
    void setID(int id) { this -> id = id;}
    void setName(const string& name){ this -> name = name;}
    void setDesc(const string& desc){ this -> desc = desc;}
    int getID() { return this -> id;}
    string getName() { return this -> name;}
    string getDesc() { return this -> desc;}
    vector<GroupUser>& getUsers() { return this -> users;}
private:
    int id;
    string name;
    string desc;
    vector<GroupUser> users;
};
#endif