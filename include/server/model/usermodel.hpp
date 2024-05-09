#ifndef USERMODEL_H
#define USERMODEL_H
using namespace std;
#include "user.hpp"
#include <string>
class UserModel{
public:
    bool insert(User&);
    User query(int);
    bool updateState(int id, const string&);
    bool reset();
private:

};
#endif