/*
 * @Date: 2023-11-02 16:35:15
 * @author: lidonghang-02 2426971102@qq.com
 * @LastEditTime: 2023-12-19 17:11:36
 */
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "sql_func.h"
#include "../Common/time.h"

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
    struct callback_result *result = (struct callback_result *)data;
    if (argc > 0 && argv[0] != NULL)
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
int check_user_exists(char *username)
{
    struct callback_result result;
    result.str = NULL;
    char sql[SQLBUFFER_SIZE];

    memset(sql, 0, SQLBUFFER_SIZE);
    sprintf(sql, "select exists(select * from User where username = %s)", username);
    if (sqlite3_exec(db, sql, callback_1, &result, &errmsg) != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }

    if (strcmp(result.str, "1") != 0)
    {
        return 0;
    }
    else
        return 1;
}
/**
 * @description: 判断群聊是否存在
 * @param {char} *groupname
 * @return {
 * -1:sql报错,
 * 0:群聊不存在,
 * 1:存在
 * }
 */
int check_group_exists(char *groupname)
{
    struct callback_result result;
    result.str = NULL;
    char sql[SQLBUFFER_SIZE];

    memset(sql, 0, SQLBUFFER_SIZE);
    sprintf(sql, "select exists(select * from ChatGroup where name = %s)", groupname);
    if (sqlite3_exec(db, sql, callback_1, &result, &errmsg) != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }

    if (strcmp(result.str, "1") != 0)
    {
        return 0;
    }
    else
        return 1;
}

/**
 * @description: 查询用户密码是否正确
 * @param {int} id 用户id
 * @param {char} *password 用户密码
 * @return {
 *-1：sql错误,
 *0：密码错误
 *1:成功
 * }
 */
int check_password(char *username, char *password)
{
    struct callback_result result;
    result.str = NULL;
    char sql[SQLBUFFER_SIZE];

    memset(sql, 0, SQLBUFFER_SIZE);
    sprintf(sql, "select exists(select * from User where username = %s and password = '%s')", username, password);
    if (sqlite3_exec(db, sql, callback_1, &result, &errmsg) != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }
    if (strcmp(result.str, "1") != 0)
    {
        return 0;
    }
    else
        return 1;
}

/**
 * @description: 查询对方是否是你的好友
 * @param {char} *username
 * @param {char} *friendname
 * @return {1：是，0：不是，-：sql错误}
 */
int check_friendship(char *username, char *friendname)
{
    struct callback_result result;
    result.str = NULL;
    char sql[SQLBUFFER_SIZE];

    memset(sql, 0, SQLBUFFER_SIZE);
    sprintf(sql, "select exists(select * from Friendship where username = %s and friendname = '%s')", username, friendname);
    if (sqlite3_exec(db, sql, callback_1, &result, &errmsg) != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }
    if (strcmp(result.str, "1") != 0)
    {
        return 0;
    }
    else
        return 1;
}
/**
 * @description: 查询是否加入该群聊
 * @param {char} *username
 * @param {char} *friendname
 * @return {1：是，0：不是，-：sql错误}
 */
int check_group_member(char *username, char *groupname)
{
    struct callback_result result;
    result.str = NULL;
    char sql[SQLBUFFER_SIZE];

    memset(sql, 0, SQLBUFFER_SIZE);
    sprintf(sql, "select exists(select * from GroupMember where group_name = %s and user_name = '%s')", groupname, username);
    if (sqlite3_exec(db, sql, callback_1, &result, &errmsg) != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }
    if (strcmp(result.str, "1") != 0)
    {
        return 0;
    }
    else
        return 1;
}

char *get_group_owner(char *groupname)
{
    struct callback_result result;
    result.str = NULL;
    char sql[SQLBUFFER_SIZE];

    // 判断用户是否已经登陆
    memset(sql, 0, SQLBUFFER_SIZE);
    sprintf(sql, "select creator_name from ChatGroup where name = %s", groupname);
    if (sqlite3_exec(db, sql, callback_1, &result, &errmsg) != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }
    return result.str;
}

/**
 * @description: 创建用户
 * @param {char} *username 用户名
 * @param {char} *password 密码
 * @return {
 * -1：sql错误,0：成功
 * }
 */
int create_user(char *username, char *password)
{
    char sql[SQLBUFFER_SIZE];
    char time_str[25];
    user_date_t date = DateNow();
    user_time_t time = TimeNow();
    sprintf(time_str, "%04d-%02d-%02d %02d:%02d:%02d", date.year, date.month, date.day, time.hour, time.minute, time.second);
    memset(sql, 0, SQLBUFFER_SIZE);
    sprintf(sql, "insert into User (username,password,status,created_at,updated_at) values('%s','%s',1,%s,%s) ", username, password, time_str, time_str);
    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }
    return 0;
}

/**
 * @description: 获取好友列表
 * @param {char} *username
 * @param {char} *msg
 * @return {*}
 */
int get_friend_list(char *username, char *msg)
{
    char sql[SQLBUFFER_SIZE];
    char **result; // 存储结果的二维数组
    int nrow;      // 结果集的行数
    int ncolumn;   // 结果集的列数
    char *errmsg;  // 错误信息
    char temp[32];
    int ret;
    memset(sql, 0, SQLBUFFER_SIZE);
    sprintf(sql, "select * from Friendship where username = %s", username);
    ret = sqlite3_get_table(db, sql, &result, &nrow, &ncolumn, &errmsg);
    if (ret != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }
    else
    {
        int j;
        for (int i = 1; i <= nrow; i++)
        {
            j = i * ncolumn;
            printf("%s\t%s\t\n", result[j + 2], result[j + 3]);
            sprintf(temp, "%s\t%s\n\n", result[j + 2], result[j + 3]);
            strcat(msg, temp);
        }
        msg[strlen(msg) - 1] = '\0';
    }
    return 0;
}

int get_group_list(char *username, char *msg)
{
    char sql[SQLBUFFER_SIZE];
    char **result; // 存储结果的二维数组
    int nrow;      // 结果集的行数
    int ncolumn;   // 结果集的列数
    char *errmsg;  // 错误信息
    char temp[32];
    int ret;
    memset(sql, 0, SQLBUFFER_SIZE);
    sprintf(sql, "select * from GroupMember where username = %s", username);
    ret = sqlite3_get_table(db, sql, &result, &nrow, &ncolumn, &errmsg);
    if (ret != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }
    else
    {
        int j;
        for (int i = 1; i <= nrow; i++)
        {
            j = i * ncolumn;
            printf("%s\t%s\t\n", result[j + 1], result[j + 4]);
            sprintf(temp, "%s\t%s\n\n", result[j + 1], result[j + 4]);
            strcat(msg, temp);
        }
        msg[strlen(msg) - 1] = '\0';
    }
    return 0;
}

/**
 * @description: 获取申请列表
 * @param {char} *username
 * @param {char} *msg
 * @return {*}
 */
void get_application(char *username, char *msg)
{
    int ret;
    char temp[64];

    char **result;            // 存储结果的二维数组
    int nrow;                 // 结果集的行数
    int ncolumn;              // 结果集的列数
    char *errmsg;             // 错误信息
    char sql[SQLBUFFER_SIZE]; // SQL 查询语句
    memset(sql, 0, sizeof(sql));
    sprintf(sql, "select * from Application where recipient_name = %s", username);

    ret = sqlite3_get_table(db, sql, &result, &nrow, &ncolumn, &errmsg);
    if (ret != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        exit(-1);
    }
    else
    {
        int j;
        for (int i = 1; i <= nrow; i++)
        {
            j = i * ncolumn;
            printf("%s\t%s\t\n", result[j + 5], result[j + 6]);
            if (atoi(result[j + 2]) == APPLY_FRIEND)
                sprintf(temp, "#%s$$%s$$%s----%s#", result[j + 1], result[j + 2], result[j + 5], result[j + 6]);
            else if (atoi(result[j + 2]) == APPLY_GROUP)
                sprintf(temp, "#%s$$%s$$%s$$%s----%s#", result[j + 1], result[j + 2], result[j + 3], result[j + 5], result[j + 6]);
            strcat(msg, temp);
        }
        msg[strlen(msg) - 1] = '\0';
    }
}

int get_message(char *username, char *target, char *msg)
{
    char sql[SQLBUFFER_SIZE];
    char **result; // 存储结果的二维数组
    int nrow;      // 结果集的行数
    int ncolumn;   // 结果集的列数
    char *errmsg;  // 错误信息
    char temp[32];
    int ret;
    memset(sql, 0, SQLBUFFER_SIZE);
    sprintf(sql, "select * from Message where (sender_name = %s and recipient_name = %s) or (sender_name = %s and recipient_name = %s)", username, target, target, username);
    ret = sqlite3_get_table(db, sql, &result, &nrow, &ncolumn, &errmsg);
    if (ret != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }
    else
    {
        int j;
        for (int i = 1; i <= nrow; i++)
        {
            j = i * ncolumn;
            printf("%s\t%s\t\n", result[j + 3], result[j + 4]);
            sprintf(temp, "%s\t%s\n\n", result[j + 3], result[j + 4]);
            strcat(msg, temp);
        }
        msg[strlen(msg) - 1] = '\0';
    }
    return 0;
}

int get_User_information(char *username, char *msg)
{
    char sql[SQLBUFFER_SIZE];
    char **result; // 存储结果的二维数组
    int nrow;      // 结果集的行数
    int ncolumn;   // 结果集的列数
    char *errmsg;  // 错误信息
    char temp[100];
    int ret;
    memset(sql, 0, SQLBUFFER_SIZE);
    sprintf(sql, "select * from User where username = %s", username);
    ret = sqlite3_get_table(db, sql, &result, &nrow, &ncolumn, &errmsg);
    if (ret != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }
    else
    {
        int j;
        for (int i = 1; i <= nrow; i++)
        {
            j = i * ncolumn;
            sprintf(temp, "1.username:%s\n2.password:%s\n3.email:%s\n4.status%s\n5.profile:%s\n6.created_at:%s\n7.updated_at%s\n\n", result[j + 1], result[j + 2], result[j + 3], result[j + 4], result[j + 5], result[j + 6], result[j + 7]);
            strcat(msg, temp);
        }
        msg[strlen(msg) - 1] = '\0';
    }
    return 0;
}

/**
 * @description: 向好友表中插入好友关系
 * @param {int} id_1
 * @param {int} id_2
 * @return {-1 sql错误;0 成功}
 */
int insert_Friendship(char *friendname, char *username)
{
    char sql[SQLBUFFER_SIZE];
    char time_str[25];
    user_date_t date = DateNow();
    user_time_t time = TimeNow();
    sprintf(time_str, "%04d-%02d-%02d %02d:%02d:%02d", date.year, date.month, date.day, time.hour, time.minute, time.second);

    sprintf(sql, "insert into Friendship (username,friendname,created_at) values(%d,%d,%s)", username, friendname, time_str);
    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }

    sprintf(sql, "insert into Friendship (username,friendname,created_at) values(%d,%d,%s)", friendname, username, time_str);
    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }
    return 0;
}

/**
 * @description: 向群聊表中插入群成员
 * @param {char} *groupname
 * @param {char} *username
 * @return {-1 sql错误;0 成功}
 */
int insert_GroupMember(char *groupname, char *username)
{
    char sql[SQLBUFFER_SIZE];
    char time_str[25];
    user_date_t date = DateNow();
    user_time_t time = TimeNow();
    sprintf(time_str, "%04d-%02d-%02d %02d:%02d:%02d", date.year, date.month, date.day, time.hour, time.minute, time.second);

    sprintf(sql, "insert into GroupMember (groupname,username,permissions,created_at) values(%s,%s,%d,%s)", groupname, username, MEMBER, time_str);
    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }
    return 0;
}

/**
 * @description: 添加申请消息
 * @param {char} *username
 * @param {char} *friendname
 * @param {int} type
 * @param {char} *msg
 * @return {0：成功;-1：sql错误}
 */
int insert_Application(char *send_name, char *recv_name, int type, char *groupname, char *msg)
{
    char sql[SQLBUFFER_SIZE];
    char time_str[25];
    user_date_t date = DateNow();
    user_time_t time = TimeNow();
    sprintf(time_str, "%04d-%02d-%02d %02d:%02d:%02d", date.year, date.month, date.day, time.hour, time.minute, time.second);
    sprintf(sql, "insert into Application (sender_name,msg_type,groupname,receiver_name,content,sent_at) values(%s,%d,%s,%s,%s,%s)", send_name, type, groupname, recv_name, msg, time_str);

    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }
    return 0;
}

/**
 * @description: 删除申请信息
 * @param {char} *sender_name
 * @param {char} *recipient_name
 * @return {*}
 */
int delete_Application(char *sender_name, char *recipient_name)
{
    char sql[SQLBUFFER_SIZE];
    sprintf(sql, "delete from Application where sender_name = %s and recipient_name = %s", sender_name, recipient_name);
    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        printf("sql error:%s\n", errmsg);
        return -1;
    }
    return 0;
}
