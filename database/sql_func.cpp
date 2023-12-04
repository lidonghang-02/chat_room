/*
 * @Date: 2023-11-02 16:35:15
 * @author: lidonghang-02 2426971102@qq.com
 * @LastEditTime: 2023-12-04 17:36:58
 */
#include <sqlite3.h>
#include <string.h>
#include <iostream>
#include "sql_func.h"
#include "../Common/time.h"

using namespace std;

sqlite3 *db;
#define SQLBUFFER_SIZE 128
char *errmsg;

int open_sql()
{
    if (sqlite3_open(DBFILE, &db) != SQLITE_OK)
    {
        printf("error:%s\n", sqlite3_errmsg(db));
        return -1;
    }
    else
    {
        printf("success: The database was opened successfully\n");
    }

    return 0;
}
int callback_1(void *data, int argc, char **argv, char **name)
{
    callback_result *result = (callback_result *)data;
    if (argc > 0 && argv[0] != nullptr)
    {
        result->str = strdup(argv[0]);
    }
    return 0;
}

int callback_2(void *data, int argc, char **argv, char **name)
{
    printf("your friends(id:username):\n");
    for (int i = 0; i < argc; i++)
    {
        printf("%s:%s\n", argv[i] ? argv[i] : "NULL", get_username(atoi(argv[i])));
    }
    return 0;
}

/**
 * @description: 检查用户是否存在
 * @param {int} id 用户ID
 * @return {
 * -1:sql报错,
 * 0:用户不存在,
 * 1:存在
 * }
 */
int check_user_exists(int ID)
{
    callback_result result;
    result.str = NULL;
    char sql[SQLBUFFER_SIZE];

    memset(sql, 0, SQLBUFFER_SIZE);
    sprintf(sql, "select exists(select * from User where id = %d)", ID);
    if (sqlite3_exec(db, sql, callback_1, &result, &errmsg) != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }
    else
    {
        if (strcmp(result.str, "1") != 0)
        {
            cout << "ERROR: User[" << ID << "] does not exist" << endl;
            return 0;
        }
    }
    return 1;
}

/**
 * @description: 查询用户密码是否正确
 * @param {int} id 用户id
 * @param {char} *password 用户密码
 * @return {
 *-1：sql错误,
 *0：用户不存在
 *1:成功
 * }
 */
int check_password(int ID, char *password)
{
    callback_result result;
    result.str = NULL;
    char sql[SQLBUFFER_SIZE];

    memset(sql, 0, SQLBUFFER_SIZE);
    sprintf(sql, "select exists(select * from user where id = %d and password = '%s')", ID, password);
    if (sqlite3_exec(db, sql, callback_1, &result, &errmsg) != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }
    else
    {
        if (strcmp(result.str, "1") != 0)
        {
            cout << "ERROR: Wrong user[" << ID << "] password" << endl;
            return 0;
        }
    }
    return 1;
}

/**
 * @description: 获取用户状态
 * @param {int} ID 用户id
 * @return {
 * -1:sql错误，
 * 其他：用户状态
 * }
 */
int get_user_status(int ID)
{
    callback_result result;
    result.str = NULL;
    char sql[SQLBUFFER_SIZE];

    // 判断用户是否已经登陆
    memset(sql, 0, SQLBUFFER_SIZE);
    sprintf(sql, "select status from user where id = %d", ID);
    if (sqlite3_exec(db, sql, callback_1, &result, &errmsg) != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }
    return atoi(result.str);
}
/**
 * @description: 修改用户状态
 * @param {int} ID 用户ID
 * @param {int} status 用户状态
 * @return {
 * -1：sql错误,
 * 0：成功
 * }
 */
int update_user_status(int ID, int status)
{
    char sql[SQLBUFFER_SIZE];
    memset(sql, 0, SQLBUFFER_SIZE);
    sprintf(sql, "update user set status = %d where id = %d", status, ID);
    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }
    return 0;
}

/**
 * @description: 创建用户
 * @param {char} *username 用户名
 * @param {char} *password 密码
 * @return {
 * -1：sql错误,
 * 其他：返回用户id
 * }
 */
int create_user(char *username, char *password)
{
    char sql[SQLBUFFER_SIZE];
    char time_str[25];
    user_date_t date = DateNow();
    user_time_t time = TimeNow();
    sprintf(time_str, "%04d-%02d-%02d %02d:%02d:%02d", date.year, date.month, date.day, time.hour, time.minute, time.second);
    cout << "time : " << time_str << endl;
    memset(sql, 0, SQLBUFFER_SIZE);
    sprintf(sql, "insert into User (username,password,status,time_str,updated_at) values('%s','%s',1,%s,%s) ", username, password, time_str, time_str);
    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }
    else
    {
        sqlite3_int64 userId = sqlite3_last_insert_rowid(db);
        return (int)userId;
    }
}

/**
 * @description: 获取用户名
 * @param {int} ID 用户id
 * @return { 返回用户名 }
 */
char *get_username(int ID)
{
    callback_result result;
    result.str = NULL;
    char sql[SQLBUFFER_SIZE];
    memset(sql, 0, SQLBUFFER_SIZE);
    sprintf(sql, "select username from User where id = %d", ID);
    if (sqlite3_exec(db, sql, callback_1, &result, &errmsg) != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        exit(-1);
    }
    return result.str;
}

/**
 * @description: 现实好友列表
 * @param {int} uid 用户id
 * @return {*}
 */
void get_friend_list(int uid)
{
    char sql[SQLBUFFER_SIZE];
    memset(sql, 0, SQLBUFFER_SIZE);
    sprintf(sql, "select friend_id from Friendship where user_id = %d", uid);
    if (sqlite3_exec(db, sql, callback_2, 0, &errmsg) != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        exit(-1);
    }
}