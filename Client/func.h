/*
 * @Date: 2023-12-13 14:40:46
 * @author: lidonghang-02 2426971102@qq.com
 * @LastEditTime: 2023-12-19 17:00:43
 */
#ifndef FUNC_H
#define FUNC_H

void add_friend(int sockfd, char *username);
void join_group(int sockfd, char *username);
char *get_personalChat_target(int sockfd, char *username);
char *get_groupChat_target(int sockfd, char *username);
void get_application(int sockfd, char *username);
void get_friendList(int sockfd, char *username);
void get_groupList(int sockfd, char *username);
void get_records(int sockfd, char *username);
void get_informations(int sockfd, char *username);
void inster_groupList(int sockfd, char *username, char *target);

#endif /* FUNC_H */