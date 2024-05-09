#include "friendmodel.hpp"
#include "db.hpp"
#include "usermodel.hpp"
#include <vector>

bool FriendModel::insert(int userid, int friendid){
    char sql[200];
    sprintf(sql, "insert into t_friend values(%d,%d)", userid, friendid);
    MySQL mysql;
    if(MYSQL* conn = mysql.connect()){
        if(mysql.update(sql)){
            return true;
        }
    }
    return false;
}

vector<User> FriendModel::query(int userid){
    char sql[200];
    sprintf(sql, "select a.id, a.name, a.state from t_user a inner join t_friend b on a.id = b.friendid where b.userid = %d", userid);
    MySQL mysql;
    vector<User> result;
    if(MYSQL* conn = mysql.connect()){
        if(MYSQL_RES* ret = mysql.query(sql)){
            while(MYSQL_ROW row = mysql_fetch_row(ret)){
                User user;
                user.setID(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                result.push_back(user);
            }
            mysql_free_result(ret);
        }
    }
    return result;
}
