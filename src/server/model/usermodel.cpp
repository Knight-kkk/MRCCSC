#include "usermodel.hpp"
#include "db.hpp"
#include <muduo/base/Logging.h>
using namespace std;

bool UserModel::insert(User &user){
    char sql[200];
    sprintf(sql, "insert into t_user(name, password, state) values('%s', '%s', '%s')", user.getName().c_str(), user.getPassword().c_str(), user.getState().c_str());
    MySQL mysql;
    if(MYSQL * conn = mysql.connect()){
        if(mysql.update(sql)){
            LOG_INFO << "Success to execute: " << sql;
            user.setID(mysql_insert_id(conn));
            return true;
        }
    }
    return false;
}

User UserModel::query(int id){
    char sql[200];
    sprintf(sql,"select * from t_user where id = %d", id);
    MySQL mysql;
    if(MYSQL* conn = mysql.connect()){
        MYSQL_RES* ret =  mysql.query(sql);
        if(ret){
            MYSQL_ROW row = mysql_fetch_row(ret);
            if(row){
                User user;
                user.setID(atoi(row[0]));
                user.setName(row[1]);
                user.setPassword(row[2]);
                user.setState(row[3]);
                return user;
            }else{
                // 查询成功，但是没有记录
                return User();
            }
            // 释放资源
            mysql_free_result(ret);
        }
        
    }
    return User();
}

bool UserModel::updateState(int id, const string& state){
    char sql[200];
    sprintf(sql,"update t_user set state = \"%s\" where id = %d", state.c_str(), id);
    MySQL mysql;
    if(MYSQL* conn = mysql.connect()){
        if(mysql.update(sql)){
            return true;
        } 
    }
    return false;
}

bool UserModel::reset(){
    char sql[200] = "update t_user set state = 'offline' where state = 'online'";
    MySQL mysql;
    if(MYSQL* conn = mysql.connect()){
        if(mysql.update(sql)){
            return true;
        }
    }
    return false;
}