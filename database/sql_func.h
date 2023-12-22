/*
 * @Date: 2023-11-02 16:35:47
 * @author: lidonghang-02 2426971102@qq.com
 * @LastEditTime: 2023-12-19 17:04:52
 */
#ifndef SQL_FUNC_H
#define SQL_FUNC_H

#define DBFILE "../database/chat_room.db"

// 用户状态
#define Offline 1 // 离线
#define Online 2  // 在线
#define Busy 3    // 忙碌
#define Stealth 4 // 隐身

enum Permissions
{
    OWNER, // 群主
    ADMIN, // 管理员
    MEMBER // 群成员
};
// 数据库申请信息表（Application）和消息表（Message）的类型
enum Table
{
    MES_PERSONAL = 1,
    MES_GROUP,
    APPLY_FRIEND,
    APPLY_GROUP
};
struct callback_result
{
    char *str;
};

int open_sql();

int check_user_exists(char *username);
int check_password(char *username, char *password);
int check_group_exists(char *groupname);
int check_friendship(char *username, char *friendname);
int check_group_member(char *username, char *groupname);

int get_user_status(char *username);
char *get_group_owner(char *groupname);
int get_friend_list(char *username, char *msg);
int get_group_list(char *username, char *msg);
void get_application(char *username, char *msg);
int get_message(char *username, char *target, char *msg);
int get_User_information(char *username, char *msg);

int create_user(char *username, char *password);

int insert_Friendship(char *friendname, char *username);
int delete_Friendship(char *friendname, char *username);
int insert_GroupMember(char *groupname, char *username);
int insert_Application(char *send_name, char *recv_name, int type, char *groupname, char *msg);
int delete_Application(char *sender_name, char *recipient_name);

#endif /* SQL_FUNC_H */