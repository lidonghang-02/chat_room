/*
 * @Date: 2023-12-12 16:15:09
 * @author: lidonghang-02 2426971102@qq.com
 * @LastEditTime: 2023-12-19 17:09:47
 */
#include "./Config/config.h"
#include <sys/select.h>

struct userdate user[FD_SETSIZE];
struct pollfd fds[USER_MAX + 1];
int user_counter = 0;
struct GroupNode *groupList = NULL;

void Login(cJSON *root, int connfd);
void SignUp(cJSON *root, int connfd);
void AddFriend(cJSON *root, int pos);
void JoinGroup(cJSON *root, int pos);
void PersonalChat(cJSON *root, int pos);
void InsertGroupList(cJSON *root, int pos);
void GroupChat(cJSON *root, int pos);
void SelectPerson(cJSON *root, int pos);
void SelectGroup(cJSON *root, int pos);
void ProcessApplication(cJSON *root, int pos);
void FriendVerifyReply(cJSON *root, int pos);
void GroupVerifyReply(cJSON *root, int pos);
void ViewFriendsList(cJSON *root, int pos);
void ViewGroupList(cJSON *root, int pos);
void ViewRecords(cJSON *root, int pos);
void ViewInformation(cJSON *root, int pos);

int main(int argc, char *argv[])
{
    if (argc <= 2)
    {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }

    // 打开数据库
    if (open_sql() == -1)
    {
        printf("Error: open sql failed.\n");
        exit(1);
    }
    else
    {
        printf("open sql successfully");
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);

    int ret = 0;

    // 服务器地址
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0)
    {
        perror("ERROR: socket");
    }

    ret = bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0)
    {
        perror("ERROR: bind");
    }

    ret = listen(listenfd, 5);
    if (ret < 0)
    {
        perror("ERROR: listen");
    }

    for (int i = 1; i <= USER_MAX; i++)
    {
        fds[i].fd = -1;
        fds[i].events = 0;
    }

    // poll监听socket
    fds[0].fd = listenfd;
    fds[0].events = POLLIN | POLLERR;
    fds[0].revents = 0;

    while (1)
    {
        ret = poll(fds, user_counter + 1, 10000);
        if (ret < 0)
        {
            perror("ERROR: poll");
            break;
        }
        else if (ret == 0 && user_counter == 0)
        {
            perror("timeout");
            break;
        }

        for (int i = 0; i < user_counter + 1; i++)
        {
            // 连接
            if ((i == 0) && (fds[0].revents & POLLIN))
            {
                struct sockaddr_in client_addr;
                socklen_t addrlen = sizeof(client_addr);
                int connfd = accept(listenfd, (struct sockaddr *)&client_addr, &addrlen);
                if (connfd < 0)
                {
                    perror("ERROR: accept");
                    continue;
                }
                if (user_counter >= USER_MAX)
                {
                    printf("ERROR: too many users\n");
                    close(connfd);
                    continue;
                }

                user_counter++;
                fds[user_counter].fd = connfd;
                fds[user_counter].events = POLLIN | POLLHUP | POLLERR;
                fds[user_counter].revents = 0;

                printf("new user login in\n");
            }
            else if (fds[i].revents & POLLERR) // 出错
            {
            }
            else if (fds[i].revents & POLLHUP) // 关闭连接
            {
            }
            else if (fds[i].revents & POLLIN) // 接收数据
            {
                int connfd = fds[i].fd;
                int recv_len = 0;
                char buf[MSG_LEN];
                memset(buf, 0, MSG_LEN);
                ret = recv(connfd, buf, MSG_LEN, 0);
                printf("收到: sockfd = %d\n%s\n", connfd, buf);

                if (ret < 0)
                {
                    // 出错，如果不是无数据可读就关闭连接
                    if (errno != EAGAIN)
                    {
                    }
                }
                else if (ret == 0)
                {
                }
                else // 收到数据
                {
                    cJSON *root = cJSON_Parse(buf);
                    cJSON *item = cJSON_GetObjectItem(root, "msgType");
                    int msgType = item->valueint;
                    switch (msgType)
                    {
                    case LOGIN:
                        Login(root, fds[i].fd);
                        break;
                    case SIGN_UP:
                        Sign_up(root, fds[i].fd);
                        break;
                    case ADD_FRIEND:
                        AddFriend(root, i);
                    case JOIN_GROUP:
                        JoinGroup(root, i);
                        break;
                    case SELECT_PERSON:
                        SelectPerson(root, i);
                        break;
                    case SELECT_GROUP:
                        SelectGroup(root, i);
                        break;
                    case PERSONAL_CHAT:
                        PersonalChat(root, i);
                        break;
                    case INSERT_GROUPLIST:
                        InsertGroupList(root, i);
                        break;
                    case PROCESS_APPLICATION:
                        ProcessApplication(root, i);
                        break;
                    case FRIEND_VERIFY_REPLY:
                        FriendVerifyReply(root, i);
                        break;
                    case GROUP_VERIFY_REPLY:
                        GroupVerifyReply(root, i);
                        break;
                    case VIEW_FRIENDS_LIST:
                        ViewFriendsList(root, i);
                        break;
                    case VIEW_GROUPS_LIST:
                        ViewGroupList(root, i);
                        break;
                    case VIEW_RECORDS:
                        ViewRecords(root, i);
                    case VIEW_INFORMATION:
                        ViewInformation(root, i);
                        break;
                    }
                }
            }
            else if (fds[i].revents & POLLOUT) // 转发数据
            {
                char *out = cJSON_Print(user[fds[i].fd].data);
                if (send(fds[i].fd, (void *)out, MSG_LEN, 0) < 0)
                {
                    perror("ERROR: send");
                    return -1;
                }
                // 写完数据需要重新注册fds[i]的可读事件
                fds[i].events |= ~POLLOUT;
                fds[i].events |= POLLIN;
            }
        }
    }
}
void Login(cJSON *root, int connfd)
{
    int ret = -1;
    // 回复消息
    cJSON *temp = cJSON_CreateObject();
    cJSON *item = cJSON_GetArrayItem(root, "username");
    char *username = item->valuestring;
    if (check_user_exists(username) == 0)
    {
        printf("ERROR: User already exists\n");
        cJSON_AddNumberToObject(temp, "msgType", RESULT);
        cJSON_AddStringToObject(temp, "msg", "ERROR: User already exists");
        char *out = cJSON_Print(temp);
        if (send(connfd, (void *)out, MSG_LEN, 0) < 0)
        {
            perror("ERROR: 请求客户端失败");
        }
        return;
    }
    item = cJSON_GetObjectItem(root, "password");
    char *password = item->valuestring;
    if ((ret = check_password(username, password)) == 0)
    {
        printf("ERROR: Wrong password\n");
        cJSON_AddNumberToObject(temp, "msgType", RESULT);
        cJSON_AddStringToObject(temp, "msg", "ERROR: Wrong password");
        char *out = cJSON_Print(temp);
        if (send(connfd, (void *)out, MSG_LEN, 0) < 0)
        {
            perror("ERROR: 请求客户端失败");
        }
        return;
    }
    else if (ret == 1)
    {
        strcpy(user[fds[user_counter].fd].username, username);
        printf("Login successful\n");
        cJSON_AddNumberToObject(temp, "msgType", RESULT);
        cJSON_AddStringToObject(temp, "msg", "Login successful");
        char *out = cJSON_Print(temp);
        if (send(connfd, (void *)out, MSG_LEN, 0) < 0)
        {
            perror("ERROR: 请求客户端失败");
        }
    }
}
void SignUp(cJSON *root, int connfd)
{
    int ret = -1;
    // 回复消息
    cJSON *temp = cJSON_CreateObject();
    // 获取消息类型
    user_counter--;
    cJSON *item = cJSON_GetArrayItem(root, "username");
    char *username = item->valuestring;
    if ((ret = check_user_exists(username)) == 1)
    {
        cJSON_AddNumberToObject(temp, "msgType", RESULT);
        cJSON_AddStringToObject(temp, "msg", "ERROR: User already exists");
        printf("ERROR: User already exists\n");
        char *out = cJSON_Print(temp);
        if (send(connfd, (void *)out, MSG_LEN, 0) < 0)
        {
            perror("ERROR: 请求客户端失败");
        }
        return;
    }
    item = cJSON_GetObjectItem(root, "password");
    char *password = item->valuestring;
    if (create_user(username, password) == 0)
    {
        printf("Sign up successful\n");
        cJSON_AddNumberToObject(temp, "msgType", RESULT);
        cJSON_AddStringToObject(temp, "msg", "Sign up successful");
        char *out = cJSON_Print(temp);
        if (send(connfd, (void *)out, MSG_LEN, 0) < 0)
        {
            perror("ERROR: 请求客户端失败");
        }
    }
}
void AddFriend(cJSON *root, int pos)
{
    int connfd = fds[pos].fd;
    cJSON *temp = cJSON_CreateObject();
    cJSON *item = cJSON_GetObjectItem(root, "friendname");
    char *friendname = item->valuestring;
    if (check_user_exists(friendname) == 0)
    {
        fds[pos].events |= ~POLLIN;
        fds[pos].events |= POLLOUT;
        cJSON_AddNumberToObject(temp, "msgType", RESULT);
        char Error[32];
        sprintf(Error, "user:%s does not exists", friendname);
        cJSON_AddStringToObject(temp, "msg", Error);
        user[connfd].data = temp;
    }
    else
    {
        item = cJSON_GetObjectItem(root, "username");
        char *username = item->valuestring;
        char msg[32];
        sprintf(msg, "User:%s requests to add you as a friend", username);
        insert_Application(username, friendname, APPLY_FRIEND, NULL, msg);
    }
}
void JoinGroup(cJSON *root, int pos)
{
    int connfd = fds[pos].fd;
    cJSON *temp = cJSON_CreateObject();
    cJSON *item = cJSON_GetObjectItem(root, "groupname");
    char *group_name = item->valuestring;
    if (check_group_exists(group_name) == 0)
    {
        fds[pos].events |= ~POLLIN;
        fds[pos].events |= POLLOUT;
        cJSON_AddNumberToObject(temp, "msgType", RESULT);
        char Error[32];
        sprintf(Error, "group:%s does not exists", group_name);
        cJSON_AddStringToObject(temp, "msg", Error);
        user[connfd].data = temp;
    }
    else
    {
        item = cJSON_GetObjectItem(root, "username");
        char *username = item->valuestring;
        char msg[32];
        sprintf(msg, "User:%s requests to join group(%s)", username, group_name);
        char *creator_name = get_group_owner(group_name);
        insert_Application(username, creator_name, APPLY_GROUP, group_name, msg);
    }
}
void PersonalChat(cJSON *root, int pos)
{
    int connfd = fds[pos].fd;
    cJSON *temp = cJSON_CreateObject();
    cJSON *item = cJSON_GetArrayItem(root, "personname");
    char *target = item->valuestring;
    for (int j = 1; j <= user_counter; j++)
    {
        if (strcmp(user[fds[j].fd].username, target) == 0)
        {
            fds[j].events |= ~POLLIN;
            fds[j].events |= POLLOUT;
            user[fds[j].fd].data = root;
            break;
        }
        if (j == user_counter)
        {
            fds[pos].events |= ~POLLIN;
            fds[pos].events |= POLLOUT;
            cJSON_AddNumberToObject(temp, "msgType", RESULT);
            char Error[32];
            sprintf(Error, "user:%s does not online", target);
            cJSON_AddStringToObject(temp, "msg", Error);
            user[connfd].data = temp;
        }
    }
}
void InsertGroupList(cJSON *root, int pos)
{
    cJSON *item = cJSON_GetArrayItem(root, "groupname");
    char *groupname = item->valuestring;
    GroupNode *p = groupList, *pre = p;
    // 成员节点
    MemberNode *new_MemNode = (MemberNode *)calloc(1, sizeof(MemberNode));
    new_MemNode->pos = pos;
    item = cJSON_GetArrayItem(root, "username");
    char *username = item->valuestring;
    strcpy(new_MemNode->username, username);
    new_MemNode->next = NULL;
    // 查找目标群聊
    while (p != NULL && strcmp(p->groupname, groupname) != 0)
    {
        pre = p;
        p = p->next;
    }

    if (p == NULL) // 当前群没有人在线
    {
        GroupNode *new_GroupNode = (GroupNode *)calloc(1, sizeof(GroupNode));
        strcpy(new_GroupNode->groupname, groupname);
        new_GroupNode->next = NULL;
        new_GroupNode->GroupMembers = new_MemNode;
        groupList = new_GroupNode;
    }
    else // 有人在当前群中聊天
    {
        MemberNode *pMember = p->GroupMembers;
        while (pMember != NULL)
        {
            pMember = pMember->next;
        }
        pMember->next = new_MemNode;
    }
}
void GroupChat(cJSON *root, int pos)
{
    int connfd = fds[pos].fd;
    cJSON *item = cJSON_GetArrayItem(root, "groupname");
    char *groupname = item->valuestring;
    item = cJSON_GetObjectItem(root, "username");
    char *username = item->valuestring;
    GroupNode *p = groupList;
    // 查找目标群聊
    while (p != NULL && strcmp(p->groupname, groupname) != 0)
    {
        p = p->next;
    }

    if (p == NULL) // 当前群没有人在线
    {
    }
    else // 有人在当前群中聊天
    {
        MemberNode *pMember = p->GroupMembers;
        while (pMember != NULL)
        {
            if (strcmp(pMember->username, username) != 0)
            {

                fds[pMember->pos].events |= ~POLLIN;
                fds[pMember->pos].events |= POLLOUT;
                user[connfd].data = root;
                pMember = pMember->next;
            }
        }
    }
}
void SelectPerson(cJSON *root, int pos)
{
    cJSON *temp = cJSON_CreateObject();
    int connfd = fds[pos].fd;
    cJSON *item = cJSON_GetArrayItem(root, "username");
    char *username = item->valuestring;
    item = cJSON_GetArrayItem(root, "target");
    char *target = item->valuestring;
    if (check_user_exists(target) == 0)
    {
        fds[pos].events |= ~POLLIN;
        fds[pos].events |= POLLOUT;
        cJSON_AddNumberToObject(temp, "msgType", RESULT);
        char Error[32];
        sprintf(Error, "ERROR: user:%s does not exists", target);
        cJSON_AddStringToObject(temp, "msg", Error);
        user[connfd].data = temp;
        return;
    }
    if (check_friendship(username, target) == 0)
    {
        fds[pos].events |= ~POLLIN;
        fds[pos].events |= POLLOUT;
        cJSON_AddNumberToObject(temp, "msgType", RESULT);
        char RESULT[32];
        sprintf(RESULT, "ERROR: user:%s is not your friend", target);
        cJSON_AddStringToObject(temp, "msg", RESULT);
        user[connfd].data = temp;
        return;
    }
    fds[pos].events |= ~POLLIN;
    fds[pos].events |= POLLOUT;
    cJSON_AddNumberToObject(temp, "msgType", SUCCESS);
    user[connfd].data = root;
}
void SelectGroup(cJSON *root, int pos)
{
    cJSON *temp = cJSON_CreateObject();
    int connfd = fds[pos].fd;
    cJSON *item = cJSON_GetArrayItem(root, "username");
    char *username = item->valuestring;
    item = cJSON_GetArrayItem(root, "target");
    char *target = item->valuestring;
    if (check_group_exists(target) == 0)
    {
        fds[pos].events |= ~POLLIN;
        fds[pos].events |= POLLOUT;
        cJSON_AddNumberToObject(temp, "msgType", RESULT);
        char Error[32];
        sprintf(Error, "ERROR: group(%s) does not exists", target);
        cJSON_AddStringToObject(temp, "msg", Error);
        user[connfd].data = temp;
        return;
    }
    if (check_group_member(username, target) == 0)
    {
        fds[pos].events |= ~POLLIN;
        fds[pos].events |= POLLOUT;
        cJSON_AddNumberToObject(temp, "msgType", RESULT);
        char RESULT[32];
        sprintf(RESULT, "ERROR: You did not join the group(%s)", target);
        cJSON_AddStringToObject(temp, "msg", RESULT);
        user[connfd].data = temp;
        return;
    }
    fds[pos].events |= ~POLLIN;
    fds[pos].events |= POLLOUT;
    cJSON_AddNumberToObject(temp, "msgType", SUCCESS);
    user[connfd].data = root;
}
void ProcessApplication(cJSON *root, int pos)
{
    cJSON *item = cJSON_GetObjectItem(root, "username");
    char *username = item->valuestring;
    char msg[512];
    get_application(username, msg);
    cJSON *temp = cJSON_CreateObject();
    fds[pos].events |= ~POLLIN;
    fds[pos].events |= POLLOUT;
    cJSON_AddNumberToObject(temp, "msgType", VERIFY);
    cJSON_AddStringToObject(temp, "msg", msg);
    user[fds[pos].fd].data = temp;
}
void FriendVerifyReply(cJSON *root, int pos)
{
    cJSON *item = cJSON_GetObjectItem(root, "username");
    char *username = item->valuestring;
    item = cJSON_GetObjectItem(root, "target");
    char *target = item->valuestring;
    item = cJSON_GetObjectItem(root, "result");
    int result = item->valueint;
    cJSON *temp = cJSON_CreateObject();
    temp = cJSON_AddNumberToObject(temp, "msgType", RESULT);
    char ret[32];
    switch (result)
    {
    case AGREE:
        insert_Friendship(username, target);
        delete_Application(target, username);
        sprintf(ret, "User:%s agrees to add you as a friend", username);
        break;
    case REJECT:
        sprintf(ret, "User:%s refuses to add you as a friend", username);
        break;
    case JUMP_OVER:
        return;
    }
    cJSON_AddStringToObject(temp, "msg", ret);
    for (int j = 1; j <= user_counter; j++)
    {
        if (strcmp(user[fds[j].fd].username, target) == 0)
        {
            fds[j].events |= ~POLLIN;
            fds[j].events |= POLLOUT;
            user[fds[j].fd].data = temp;
            break;
        }
    }
}
void GroupVerifyReply(cJSON *root, int pos)
{
    cJSON *item = cJSON_GetObjectItem(root, "username");
    char *username = item->valuestring;
    item = cJSON_GetObjectItem(root, "groupname");
    char *groupname = item->valuestring;
    item = cJSON_GetObjectItem(root, "target");
    char *target = item->valuestring;
    item = cJSON_GetObjectItem(root, "result");
    int result = item->valueint;
    cJSON *temp = cJSON_CreateObject();
    temp = cJSON_AddNumberToObject(temp, "msgType", RESULT);
    char ret[32];
    switch (result)
    {
    case AGREE:
        insert_GroupMember(groupname, username);
        delete_Application(target, username);
        sprintf(ret, "User(%s) agrees to join group chat(%s)", username, groupname);
        break;
    case REJECT:
        sprintf(ret, "User(%s) refuses to join group chat(%s)", username, groupname);
        break;
    case JUMP_OVER:
        return;
    }
    cJSON_AddStringToObject(temp, "msg", ret);
    for (int j = 1; j <= user_counter; j++)
    {
        if (strcmp(user[fds[j].fd].username, target) == 0)
        {
            fds[j].events |= ~POLLIN;
            fds[j].events |= POLLOUT;
            user[fds[j].fd].data = temp;
            break;
        }
    }
}
void ViewFriendsList(cJSON *root, int pos)
{
    char msg[512];
    cJSON *item = cJSON_GetArrayItem(root, "username");
    char *username = item->valuestring;
    get_friend_list(username, msg);
    char *temp = cJSON_CreateObject();
    cJSON_AddNumberToObject(temp, "msgType", RESULT);
    cJSON_AddStringToObject(temp, "msg", msg);
    fds[pos].events |= ~POLLIN;
    fds[pos].events |= POLLOUT;
    user[fds[pos].fd].data = temp;
}
void ViewGroupList(cJSON *root, int pos)
{
    char msg[512];
    cJSON *item = cJSON_GetArrayItem(root, "username");
    char *username = item->valuestring;
    get_group_list(username, msg);
    char *temp = cJSON_CreateObject();
    cJSON_AddNumberToObject(temp, "msgType", RESULT);
    cJSON_AddStringToObject(temp, "msg", msg);
    fds[pos].events |= ~POLLIN;
    fds[pos].events |= POLLOUT;
    user[fds[pos].fd].data = temp;
}
void ViewRecords(cJSON *root, int pos)
{
    char msg[1000];
    cJSON *item = cJSON_GetArrayItem(root, "username");
    char *username = item->valuestring;
    item = cJSON_GetArrayItem(root, "target");
    char *target = item->valuestring;
    get_message(username, target, msg);
    char *temp = cJSON_CreateObject();
    cJSON_AddNumberToObject(temp, "msgType", RESULT);
    cJSON_AddStringToObject(temp, "msg", msg);
    fds[pos].events |= ~POLLIN;
    fds[pos].events |= POLLOUT;
    user[fds[pos].fd].data = temp;
}
void ViewInformation(cJSON *root, int pos)
{
    char msg[256];
    cJSON *item = cJSON_GetArrayItem(root, "username");
    char *username = item->valuestring;
    get_User_information(username, msg);
    char *temp = cJSON_CreateObject();
    cJSON_AddNumberToObject(temp, "msgType", RESULT);
    cJSON_AddStringToObject(temp, "msg", msg);
    fds[pos].events |= ~POLLIN;
    fds[pos].events |= POLLOUT;
    user[fds[pos].fd].data = temp;
}
