#ifndef MSG_TYPR_H
#define MSG_TYPR_H

#define MSG_LEN 1024
#define USER_MAX 32

// 消息类型
#define LOGIN 1
#define LOGOUT 2
#define ADD_FRIEND 3
#define REPLY 4

// 当前client状态
#define MAIN_UI 1

struct msg
{
    int send_id;
    char name[16];
    char sned_time[32];
    char text[128];
    struct msg *next;
} Apply, private_chat, group_chat;

#endif /* MSG_TYPR_H */