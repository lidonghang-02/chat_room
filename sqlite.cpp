/*
 * @Date: 2023-10-22 13:20:32
 * @author: lidonghang-02 2426971102@qq.com
 * @LastEditTime: 2023-10-30 17:46:02
 */
#include <sqlite3.h>
#include <string.h>
#include <iostream>
#include "chat_room.h"

using namespace std;

// sqlite3的库是第三方提供的动态库，所以编译时需要手动链接库文件 -lsqlite3
// gcc sqlite.c -lsqlite3 -o sqlite

#define DBFILE "chat_room.db"
#define SQLBUFFER_SIZE 128
struct callback_result
{
    char *str;
};

/*
user 存储用户信息： 用户编号-uid  用户名name  密码passwd  状态state(0未登录 1在线)
friends 存储用户好友： 用户编号uid 用户好友编号fid
*/
sqlite3 *db;
char sql[SQLBUFFER_SIZE] = {0};

char *errmsg;

int callback(void *data, int argc, char **argv, char **name)
{
    // int *result = static_cast<int *>(data);
    callback_result *result = (callback_result *)data;
    if (argc > 0 && argv[0] != nullptr)
    {
        // *result = std::stoi(argv[0]); // 将结果转换为整数
        result->str = strdup(argv[0]);
    }
    return 0;
}

int Init_SQL()
{
    // 创建或者打开一个数据库
    if (sqlite3_open(DBFILE, &db) != SQLITE_OK)
    {
        printf("sql error:%s\n", sqlite3_errmsg(db));
        return -1;
    }
    else
    {
        printf("成功：数据库已经打开或者创建成功了\n");
    }
    // 建表
    strcpy(sql, "create table user(uid integer primary key autoincrement,name varchar(50),passwd varchar(50),state integer)");
    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        printf("sql error%s\n", errmsg);
        return -1;
    }
    else
    {
        printf("table user create success\n");
    }
    strcpy(sql, "create table friends(uid int,fid int,FOREIGN KEY(uid) REFERENCES user(uid),FOREIGN KEY(fid) REFERENCES user(uid))");
    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        printf("error%s\n", errmsg);
        return -1;
    }
    else
    {
        printf("table friends create success\n");
    }
    return 0;
}
/**
 * @description: 检查用户是否存在
 * @param {int} id
 * @return {-1 sql报错       0 用户不存在       1 存在}
 */
int check_user_exists(int id)
{
    callback_result result;
    result.str = NULL;
    memset(sql, 0, SQLBUFFER_SIZE);
    sprintf(sql, "select exists(select * from user where uid = %d)", id);
    if (sqlite3_exec(db, sql, callback, &result, &errmsg) != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }
    else
    {
        if (strcmp(result.str, "1") != 0)
        {
            return 0;
        }
    }
    return 1;
}

// 创建用户
// 返回值：失败返回-1, 成功返回用户id（id>0）
// 输入用户名和密码，ID自动分配，登陆时使用ID和password
int create_user(char *name, int &id)
{
    char username[32], password[32];
    callback_result result;
    result.str = NULL;
    printf("Please enter your username:");
    scanf("%s", username);
    while (strlen(username) > 10)
    {
        cout << "username too long" << endl;
        cout << "Please enter your username again" << endl;
        scanf("%s", username);
    }
    strcpy(name, username);
    printf("Please enter your password:");
    while (scanf("%s", password))
    {
        if (strlen(password) > 10)
        {
            cout << "password too long" << endl;
            cout << "Please enter your password again" << endl;
        }
        else
            break;
    }

    memset(sql, 0, SQLBUFFER_SIZE);
    sprintf(sql, "insert into user (name,passwd,state) values('%s','%s',0)", username, password);
    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        printf("create user sql error:%s\n", errmsg);
        return -1;
    }
    memset(sql, 0, SQLBUFFER_SIZE);
    strcpy(sql, "SELECT MAX(uid) FROM user");
    if (sqlite3_exec(db, sql, callback, &result, &errmsg) != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }
    else
    {
        printf("--------------------------\n");
        printf("user[%s] create success\n", result.str);
        printf("your UID is %s\n", result.str);
        printf("The UID is used to login\n");
        printf("--------------------------\n");
    }
    id = atoi(result.str);
    return 0;
}
// 添加好友
// 返回值： 0 成功     -1 失败
int add_friend(int UID, int FID)
{
    // int ret;
    // ret = check_user_exists(FID);
    // if (ret == -1)
    // {
    //     return -1;
    // }
    // else if (ret == 0)
    // {
    //     cout << "user[" << FID << "] does not exist" << endl;
    //     return -1;
    // }

    // 将好友关系插入数据库
    memset(sql, 0, SQLBUFFER_SIZE);
    sprintf(sql, "insert into friends values(%d,%d)", UID, FID);
    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }
    else
    {
        cout << "add friend[" << FID << "] successfully" << endl;
    }
    return 0;
}

// 登陆
int login(char *name, int &id)
{
    int UID, ret = -1;
    char password[32];
    callback_result result;
    result.str = NULL;

    // 查询UID是否正确
    cout << "Please enter your UID:";
    cin >> UID;
    ret = check_user_exists(UID);
    if (ret == -1)
    {
        return -1;
    }
    else if (ret == 0)
    {
        cout << "user[" << UID << "] does not exist" << endl;
        return -1;
    }

    // 查询passwd是否正确
    cout << "Please enter your passwd:";
    scanf("%s", password);
    memset(sql, 0, SQLBUFFER_SIZE);
    sprintf(sql, "select exists(select * from user where uid = %d and passwd = '%s')", UID, password);
    if (sqlite3_exec(db, sql, callback, &result, &errmsg) != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }
    else
    {
        if (strcmp(result.str, "1") != 0)
        {
            cout << "Wrong user[" << UID << "] password" << endl;
            return -1;
        }
    }

    // 判断用户是否已经登陆
    memset(sql, 0, SQLBUFFER_SIZE);
    sprintf(sql, "select state from user where uid = %d", UID);
    if (sqlite3_exec(db, sql, callback, &result, &errmsg) != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }
    else
    {
        if (strcmp(result.str, "1") == 0)
        {
            printf("warning:user %d has login in\n", UID);
            return -1;
        }
        else
        {
            // 修改用户state
            memset(sql, 0, SQLBUFFER_SIZE);
            sprintf(sql, "update user set state = 1 where uid = %d", UID);
            if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
            {
                printf("sql error:%s\n", errmsg);
                return -1;
            }
        }
    }

    // 查找用户名
    memset(sql, 0, SQLBUFFER_SIZE);
    sprintf(sql, "select name from user where uid = %d", UID);
    if (sqlite3_exec(db, sql, callback, &result, &errmsg) != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }

    strcpy(name, result.str);
    id = UID;
    cout << "login success" << endl;
    return 0;
}
// 用户退出
int user_quit(int UID)
{

    callback_result result;
    result.str = NULL;
    memset(sql, 0, SQLBUFFER_SIZE);
    sprintf(sql, "select state from user where uid = %d", UID);
    if (sqlite3_exec(db, sql, callback, &result, &errmsg) != SQLITE_OK)
    {
        printf("sql errdsadasdasor:%s\n", errmsg);
        return -1;
    }
    else
    {
        if (strcmp(result.str, "0") == 0)
        {
            printf("warning:user %d has login in\n", UID);
            return -1;
        }
        else
        {
            // 修改用户state
            memset(sql, 0, SQLBUFFER_SIZE);
            sprintf(sql, "update user set state = 0 where uid = %d", UID);
            if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
            {
                printf("sql error:%s\n", errmsg);
                return -1;
            }
        }
    }
    // 修改用户state
    // memset(sql, 0,SQLBUFFER_SIZE);
    // sprintf(sql, "update user set state = 0 where uid = %d", UID);
    // if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
    // {
    //     cout << "sql error: " << sqlite3_errmsg(db) << endl;
    //     // printf("sql error:%s\n", errmsg);
    //     return -1;
    // }
    return 0;
}
// 判断私聊对象是否存在和是否已经添加好友
int is_friend(int UID, int FID)
{
    callback_result ret_1, ret_2;
    ret_1.str = NULL;
    ret_2.str = NULL;

    int ret = check_user_exists(FID);
    if (ret == -1)
    {
        return -1;
    }
    else if (ret == 0)
    {
        cout << "user(" << FID << ") does not exist" << endl;
        return -1;
    }

    memset(sql, 0, SQLBUFFER_SIZE);
    sprintf(sql, "select exists(select * from friends where uid = %d and fid = %d)", UID, FID);
    if (sqlite3_exec(db, sql, callback, &ret_1, &errmsg) != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }

    memset(sql, 0, SQLBUFFER_SIZE);
    sprintf(sql, "select exists(select * from friends where uid = %d and fid = %d)", FID, UID);
    if (sqlite3_exec(db, sql, callback, &ret_2, &errmsg) != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }

    if (strcmp(ret_1.str, "1") != 0 && strcmp(ret_2.str, "1") != 0)
    {
        cout << "user(" << FID << ") is not your friend" << endl;
        return -1;
    }

    return 0;
}

// 获取私聊对象状态
int get_friend_state(int FID)
{
    callback_result result;
    result.str = NULL;

    int ret = check_user_exists(FID);
    if (ret == -1)
    {
        return -1;
    }
    else if (ret == 0)
    {
        cout << "user(" << FID << ") does not exist" << endl;
        return -1;
    }

    memset(sql, 0, SQLBUFFER_SIZE);
    sprintf(sql, "select state from user where uid = %d", FID);
    if (sqlite3_exec(db, sql, callback, &result, &errmsg) != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }
    else
    {
        if (strcmp(result.str, "0") == 0)
        {
            return NOT_ONLINE;
        }
        else if (strcmp(result.str, "1") == 0)
        {
            return ONLINE;
        }
    }
    return -1;
}