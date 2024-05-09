#ifndef DB_H
#define DB_H
#include <muduo/base/Logging.h>
#include <mysql/mysql.h>
#include <string>
using namespace std;

// 数据库配置信息
static string server = "127.0.0.1";
static string user = "root";
static string password = ""; // MySQL密码
static string dbname = "";   // 数据库名

// 数据库操作类
class MySQL{
public:

    // 初始化数据库连接
    MySQL(); 

    // 释放数据库连接资源
    ~MySQL();

    // 连接数据库
    MYSQL* connect();

    // 更新操作
    bool update(string sql);

    // 查询操作
    MYSQL_RES* query(string sql);

private:
    MYSQL *_conn;
};
#endif