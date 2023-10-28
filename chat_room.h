/*
 * @Date: 2023-10-15 20:11:50
 * @author: lidonghang-02 2426971102@qq.com
 * @LastEditTime: 2023-10-28 14:55:03
 */
#ifndef CHAT_ROOM_H
#define CHAT_ROOM_H

#define USER_MAX 5      // 最大用户数量
#define BUFFER_SIZE 128 // 读缓冲区大小

#define Private_Chat 1
#define Public_Chat 2
#define LOGIN 3

typedef struct
{
    int opcode;            // 操作码
    char name[32];         // 用户名
    int UID;               // 用户id
    int FID;               // 私聊目标id
    char buf[BUFFER_SIZE]; // 客户端数据
} MSG;

// 数据库
// extern int uid;

int Init_SQL();                       // 数据库初始化
int create_user(char *name, int &id); // 注册
int add_friend(int id);               // 添加好友
int login(char *name, int &id);       // 登陆
int pri_chat_sql(int UID, int FID);   // 私聊

#endif /* CHAT_ROOM_HH */
