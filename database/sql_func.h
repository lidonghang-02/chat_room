/*
 * @Date: 2023-11-02 16:35:47
 * @author: lidonghang-02 2426971102@qq.com
 * @LastEditTime: 2023-12-04 17:36:10
 */
#ifndef SQL_FUNC_H
#define SQL_FUNC_H

#define DBFILE "../database/chat_room.db"

// 用户状态
#define Offline 1 // 离线
#define Online 2  // 在线
#define Busy 3    // 忙碌
#define Stealth 4 // 隐身

struct callback_result
{
    char *str;
};

int open_sql();
int check_user_exists(int id);
int check_password(int id, char *password);
int get_user_status(int ID);
int update_user_status(int ID, int status);
int create_user(char *username, char *password);
char *get_username(int ID);
void get_friend_list(int uid);

#endif /* SQL_FUNC_H */