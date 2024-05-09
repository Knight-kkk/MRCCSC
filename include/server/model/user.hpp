#ifndef USER_H
#define USER_H
#include <string>
using namespace std;
class User{
public:
    // 构造函数
    User(int id = -1, const string& name = "", const string& password = "", const string& state = "offline"){
        this -> id = id;
        this -> name = name;
        this -> password = password;
        this -> state = state;
    }
    // set函数
    void setID(int id){
        this -> id = id;
    }
    void setName(const string& name){
        this -> name = name;
    }
    void setPassword(const string& password){
        this -> password = password;
    }
    void setState(const string& state){
        this -> state = state;
    } 
    // get函数
    int getID(){
        return this -> id;
    } 
    string getName(){
        return this -> name;
    }
    string getPassword(){
        return this -> password;
    }
    string getState(){
        return this -> state;
    }
    
protected:
    int id; 
    string name;
    string password;
    string state;
};
#endif