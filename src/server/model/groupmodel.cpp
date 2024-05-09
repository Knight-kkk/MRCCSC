#include "groupmodel.hpp"
#include <string>
#include <vector>
#include "db.hpp"
#include "group.hpp"

using namespace std;

bool GroupModel::create(Group& group){
    char sql[200] = {0};
    sprintf(sql, "insert into t_group(groupname,groupdesc) values('%s','%s')", group.getName().c_str(), group.getDesc().c_str());
    MySQL mysql;
    if(MYSQL* conn = mysql.connect()){
        if(mysql.update(sql)){
            int id = mysql_insert_id(conn);
            group.setID(id);
            return true;
        }
    }
    return false;
}

bool GroupModel::join(int userid, int groupid, const string& role){
    char sql[200] = {0};
    sprintf(sql, "insert into t_groupUser values(%d, %d,'%s')", groupid, userid, role.c_str());
    MySQL mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            return true;
        }
    }
    return false;
}

vector<Group> GroupModel::queryGroups(int userid){
    char sql[200] = {0};
    sprintf(sql, "select a.id, a.groupname, a.groupdesc from t_group a inner join t_groupUser b on a.id = b.groupid where b.userid = %d", userid);
    MySQL mysql;
    vector<Group> result;
    if(mysql.connect()){
        if(MYSQL_RES* ret =  mysql.query(sql)){
            while(MYSQL_ROW row = mysql_fetch_row(ret)){
                Group group;
                group.setID(atoi(row[0]));
                group.setName(row[1]);
                group.setDesc(row[2]);
                result.push_back(group);
            }
            mysql_free_result(ret);
        }
    }
    return result;
}

vector<int> GroupModel::queryGroupUsers(int userid, int groupid){
    char sql[200] = {0};
    sprintf(sql, "select userid from t_groupUser where groupid = %d and userid != %d", groupid, userid);
    MySQL mysql;
    vector<int> result;
    if(mysql.connect()){
        if(MYSQL_RES* ret = mysql.query(sql)){
            while(MYSQL_ROW row = mysql_fetch_row(ret)){
                result.push_back(atoi(row[0]));
            }
            mysql_free_result(ret);
        }
    }
    return result;
}