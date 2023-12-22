/*
 * @Date: 2023-12-13 14:27:48
 * @author: lidonghang-02 2426971102@qq.com
 * @LastEditTime: 2023-12-19 17:01:09
 */
#include "./Config/config.h"
#include "./func.h"
void add_friend(int sockfd, char *username)
{
    char friend[32];

    printf("Please enter your friends' username");
    scanf("%s", friend);

    cJSON *temp = cJSON_CreateObject();
    cJSON_AddNumberToObject(temp, "msgType", ADD_FRIEND);
    cJSON_AddStringToObject(temp, "username", username);
    cJSON_AddStringToObject(temp, "friendname", friend);
    char *out = cJSON_Print(temp);
    if (send(sockfd, (void *)out, MSG_LEN, 0) < 0)
    {
        perror("ERROR: 请求服务器失败");
        exit(1);
    }
}

void join_group(int sockfd, char *username)
{
    char group_name[32];

    printf("Please enter group's username");
    scanf("%s", group_name);

    cJSON *temp = cJSON_CreateObject();
    cJSON_AddNumberToObject(temp, "msgType", JOIN_GROUP);
    cJSON_AddStringToObject(temp, "username", username);
    cJSON_AddStringToObject(temp, "groupname", group_name);
    char *out = cJSON_Print(temp);
    if (send(sockfd, (void *)out, MSG_LEN, 0) < 0)
    {
        perror("ERROR: 请求服务器失败");
        exit(1);
    }
}
char *get_personalChat_target(int sockfd, char *username)
{
    char personalChat_name[32];
    printf("Please enter the username of your personal chat target\n");
stp:
    scanf("%s", personalChat_name);
    cJSON *temp = cJSON_CreateObject();
    cJSON_AddNumberToObject(temp, "msgType", SELECT_PERSON);
    cJSON_AddStringToObject(temp, "username", username);
    cJSON_AddStringToObject(temp, "target", personalChat_name);
    char *out = cJSON_Print(temp);
    if (send(sockfd, (void *)out, MSG_LEN, 0) < 0)
    {
        perror("ERROR: 请求服务器失败");
        exit(1);
    }
    char buf[MSG_LEN];
    if (recv(sockfd, buf, MSG_LEN, 0) < 0)
    {
        perror("ERROR: 请求服务器失败");
        exit(1);
    }
    cJSON *root = cJSON_Parse(buf);
    cJSON *item = cJSON_GetArrayItem(root, "msgType");
    if (item->valueint == SUCCESS)
    {

        return personalChat_name;
    }
    else
    {
        item = cJSON_GetArrayItem(root, "msg");
        printf("%s\n", item->valuestring);
        printf("Whether to re-enter(Y/N)\n");
        scanf("%s", personalChat_name);
        if (strcmp(personalChat_name, "N") == 0)
            return personalChat_name;
        printf("the other party name: ");
        goto stp;
    }
    return personalChat_name;
}

char *get_groupChat_target(int sockfd, char *username)
{
    char groupChat_name[32];
    printf("Please enter the username of your group chat target\n");
stp:
    scanf("%s", groupChat_name);
    cJSON *temp = cJSON_CreateObject();
    cJSON_AddNumberToObject(temp, "msgType", SELECT_PERSON);
    cJSON_AddStringToObject(temp, "username", username);
    cJSON_AddStringToObject(temp, "target", groupChat_name);
    char *out = cJSON_Print(temp);
    if (send(sockfd, (void *)out, MSG_LEN, 0) < 0)
    {
        perror("ERROR: 请求服务器失败");
        exit(1);
    }
    char buf[MSG_LEN];
    if (recv(sockfd, buf, MSG_LEN, 0) < 0)
    {
        perror("ERROR: 请求服务器失败");
        exit(1);
    }
    cJSON *root = cJSON_Parse(buf);
    cJSON *item = cJSON_GetArrayItem(root, "msgType");
    if (item->valueint == SUCCESS)
    {

        return groupChat_name;
    }
    else
    {
        item = cJSON_GetArrayItem(root, "msg");
        printf("%s\n", item->valuestring);

        printf("You did not join the group chat\n");
        printf("Whether to re-enter(Y/N)\n");
        scanf("%s", groupChat_name);
        if (strcmp(groupChat_name, "N") == 0)
            return groupChat_name;
        printf("the other party name: ");
        goto stp;
    }
    return groupChat_name;
}

void get_application(int sockfd, char *username)
{
    cJSON *temp = cJSON_CreateObject();
    cJSON_AddNumberToObject(temp, "msgType", PROCESS_APPLICATION);
    cJSON_AddStringToObject(temp, "username", username);
    char *out = cJSON_Print(temp);
    if (send(sockfd, (void *)out, MSG_LEN, 0) < 0)
    {
        perror("ERROR: 请求服务器失败");
        exit(1);
    }
}
void get_friendList(int sockfd, char *username)
{
    cJSON *temp = cJSON_CreateObject();
    cJSON_AddNumberToObject(temp, "msgType", VIEW_FRIENDS_LIST);
    cJSON_AddStringToObject(temp, "username", username);
    char *out = cJSON_Print(temp);
    if (send(sockfd, (void *)out, MSG_LEN, 0) < 0)
    {
        perror("ERROR: 请求服务器失败");
        exit(1);
    }
}
void get_groupList(int sockfd, char *username)
{
    cJSON *temp = cJSON_CreateObject();
    cJSON_AddNumberToObject(temp, "msgType", VIEW_GROUPS_LIST);
    cJSON_AddStringToObject(temp, "username", username);
    char *out = cJSON_Print(temp);
    if (send(sockfd, (void *)out, MSG_LEN, 0) < 0)
    {
        perror("ERROR: 请求服务器失败");
        exit(1);
    }
}
void get_records(int sockfd, char *username)
{
    char target[32];
    printf("Please enter the target's username\n");
    scanf("%s", target);
    cJSON *temp = cJSON_CreateObject();
    cJSON_AddNumberToObject(temp, "msgType", VIEW_RECORDS);
    cJSON_AddStringToObject(temp, "username", username);
    cJSON_AddStringToObject(temp, "target", target);
    char *out = cJSON_Print(temp);
    if (send(sockfd, (void *)out, MSG_LEN, 0) < 0)
    {
        perror("ERROR: 请求服务器失败");
        exit(1);
    }
}

void get_informations(int sockfd, char *username)
{
    cJSON *temp = cJSON_CreateObject();
    cJSON_AddNumberToObject(temp, "msgType", VIEW_INFORMATION);
    cJSON_AddStringToObject(temp, "username", username);
    char *out = cJSON_Print(temp);
    if (send(sockfd, (void *)out, MSG_LEN, 0) < 0)
    {
        perror("ERROR: 请求服务器失败");
        exit(1);
    }
}

void inster_groupList(int sockfd, char *username, char *target)
{
    cJSON *temp = cJSON_CreateObject();
    cJSON_AddNumberToObject(temp, "msgType", INSERT_GROUPLIST);
    cJSON_AddStringToObject(temp, "username", username);
    cJSON_AddStringToObject(temp, "groupname", target);
    char *out = cJSON_Print(temp);
    if (send(sockfd, (void *)out, MSG_LEN, 0) < 0)
    {
        perror("ERROR: 请求服务器失败");
        exit(1);
    }
}
