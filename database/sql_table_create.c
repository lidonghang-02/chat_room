/*
 * @Date: 2023-11-02 16:19:21
 * @author: lidonghang-02 2426971102@qq.com
 * @LastEditTime: 2023-11-06 17:18:11
 */
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "sql_func.h"

int main()
{
    sqlite3 *db;
    char *error_message = 0;

    // 打开数据库连接
    int rc = sqlite3_open("chat_room.db", &db);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "无法创建数据库: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    // 创建用户表
    char *sql_user = "CREATE TABLE User ("
                     "id INTEGER PRIMARY KEY,"
                     "username TEXT,"
                     "password TEXT,"
                     "email TEXT,"
                     "status INTEGER,"
                     "personal_profile TEXT,"
                     "created_at DATETIME,"
                     "updated_at DATETIME"
                     ");";

    // 创建好友关系表
    char *sql_friendship = "CREATE TABLE Friendship ("
                           "id INTEGER PRIMARY KEY,"
                           "user_id INTEGER,"
                           "friend_id INTEGER,"
                           "created_at DATETIME,"
                           "FOREIGN KEY(user_id) REFERENCES User(id),"
                           "FOREIGN KEY(friend_id) REFERENCES User(id)"
                           ");";

    // 创建群组表
    char *sql_group = "CREATE TABLE ChatGroup ("
                      "id INTEGER PRIMARY KEY,"
                      "name TEXT,"
                      "creator_id INTEGER,"
                      "announcement TEXT,"
                      "created_at DATETIME,"
                      "updated_at DATETIME,"
                      "FOREIGN KEY(creator_id) REFERENCES User(id)"
                      ");";

    // 创建群组成员表
    char *sql_group_member = "CREATE TABLE GroupMember ("
                             "id INTEGER PRIMARY KEY,"
                             "group_id INTEGER,"
                             "user_id INTEGER,"
                             "created_at DATETIME,"
                             "updated_at DATETIME,"
                             "FOREIGN KEY(group_id) REFERENCES ChatGroup(id),"
                             "FOREIGN KEY(user_id) REFERENCES User(id)"
                             ");";

    // 创建消息表
    char *sql_message = "CREATE TABLE Message ("
                        "id INTEGER PRIMARY KEY,"
                        "sender_id INTEGER,"
                        "recipient_id INTEGER,"
                        "content TEXT,"
                        "sent_at DATETIME,"
                        "FOREIGN KEY(sender_id) REFERENCES User(id),"
                        "FOREIGN KEY(recipient_id) REFERENCES User(id)"
                        ");";

    // 执行创建表格的 SQL 语句
    rc = sqlite3_exec(db, sql_user, 0, 0, &error_message);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "无法执行 SQL 语句: %s\n", error_message);
        sqlite3_free(error_message);
    }

    rc = sqlite3_exec(db, sql_friendship, 0, 0, &error_message);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "无法执行 SQL 语句: %s\n", error_message);
        sqlite3_free(error_message);
    }

    rc = sqlite3_exec(db, sql_group, 0, 0, &error_message);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "无法执行 SQL 语句: %s\n", error_message);
        sqlite3_free(error_message);
    }

    rc = sqlite3_exec(db, sql_group_member, 0, 0, &error_message);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "无法执行 SQL 语句: %s\n", error_message);
        sqlite3_free(error_message);
    }

    rc = sqlite3_exec(db, sql_message, 0, 0, &error_message);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "无法执行 SQL 语句: %s\n", error_message);
        sqlite3_free(error_message);
    }
    else
    {
        printf("数据库表格创建成功！\n");
    }
    // 关闭数据库连接
    sqlite3_close(db);

    return 0;
}