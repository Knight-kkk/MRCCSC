#include "offlinemessagemodel.hpp"
#include "db.hpp"
using namespace std;

bool OfflineMsgModel::insert(int id, const string& msg){
    char sql[200];
    sprintf(sql, "insert into t_offlineMessage values(%d, '%s')", id, msg.c_str());
    MySQL mysql;
    if(MYSQL * conn = mysql.connect()){
        if(mysql.update(sql)){
            LOG_INFO << "Success to execute: " << sql;
            return true;
        }
    }
    return false;    
}

vector<string> OfflineMsgModel::query(int id){
    char sql[200];
    sprintf(sql, "select message from t_offlineMessage where userid = %d", id);
    MySQL mysql;
    vector<string> msg;
    if(MYSQL * conn = mysql.connect()){
        MYSQL_RES* ret =  mysql.query(sql);
        if(ret){
            while(MYSQL_ROW row = mysql_fetch_row(ret)){
                msg.push_back(row[0]);
            }
        }
        mysql_free_result(ret);
    }
    return msg;
}

bool OfflineMsgModel::remove(int id){
    char sql[200];
    sprintf(sql, "delete from t_offlineMessage where userid = %d", id);
    MySQL mysql;
    if(MYSQL* conn = mysql.connect()){
        if(mysql.update(sql)){
            LOG_INFO << "Success to execute: " << sql;
            return true;            
        }
    }
    return false;
}
