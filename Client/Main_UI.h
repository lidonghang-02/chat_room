/*
 * @Date: 2023-11-06 20:08:02
 * @author: lidonghang-02 2426971102@qq.com
 * @LastEditTime: 2023-12-04 14:42:40
 */
#ifndef Main_UI_H
#define Main_UI_H

struct msg
{
    int send_id;
    char name[16];
    char sned_time[32];
    char text[128];
    struct msg *next;
} Apply, private_chat, group_chat;

void Main_UI();
int Main_select(int sockfd, int uid, char *buf, int status);

#endif /* Main_UI_H */