/*
 * @Date: 2023-11-10 13:10:34
 * @author: lidonghang-02 2426971102@qq.com
 * @LastEditTime: 2023-11-20 17:38:57
 */
#include <iostream>
#include <netinet/in.h>
#include <poll.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

#include "../Common/msg_type.h"
#include "../cJSON/cJSON.h"
using namespace std;

struct userdate
{
    sockaddr_in address; // 客户端地址
    int UID;
    char name[16];
    cJSON *data;
};

int main(int argc, char *argv[])
{
    if (argc <= 2)
    {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
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

    userdate *user = new userdate[FD_SETSIZE];
    pollfd fds[USER_MAX + 1];
    int user_counter = 0;

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
            if ((fds[i].fd == listenfd) && (fds[i].revents & POLLIN))
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
                user[connfd].address = client_addr;

                fds[user_counter].fd = connfd;
                fds[user_counter].events = POLLIN | POLLRDHUP | POLLERR;
                fds[user_counter].revents = 0;

                printf("new user login in\n");
            }
            else if (fds[i].revents & POLLERR) // 出错
            {
                printf("get an error from %d\n", fds[i].fd);
            }
            else if (fds[i].revents & POLLHUP) // 关闭连接
            {
                user[fds[i].fd] = {};
                close(fds[i].fd);
                fds[i] = fds[user_counter]; // poll监听数组也要更改
                user_counter--;
                i--; // 删除了一个，i要往前面走一个
                printf("a client left\n");
            }
            else if (fds[i].revents & POLLIN) // 接收数据
            {
                int connfd = fds[i].fd;
                int recv_len = 0;
                char buf[MSG_LEN];
                memset(buf, '\0', MSG_LEN);
                ret = recv(connfd, buf, MSG_LEN, 0);
                printf("收到: sockfd = %d\n%s\n", connfd, buf);

                if (ret < 0)
                {
                    // 出错，如果不是无数据可读就关闭连接
                    if (errno != EAGAIN)
                    {
                        close(connfd);
                        user[connfd] = {};
                        fds[i] = fds[user_counter];
                        user_counter--;
                        i--;
                    }
                }
                else if (ret == 0)
                {
                    // 连接已经正常关闭
                    printf("山单位i啊和丢哦afhuis \n");
                    close(connfd);
                    user[connfd] = {};
                    fds[i] = fds[user_counter];
                    user_counter--;
                    i--;
                }
                else // 收到数据
                {
                    cJSON *root = cJSON_Parse(buf);
                    cJSON *item = cJSON_GetObjectItem(root, "send_id");
                    user[connfd].UID = item->valueint;
                    item = cJSON_GetObjectItem(root, "username");
                    strcpy(user[connfd].name, item->valuestring);

                    // 获取操作码
                    item = cJSON_GetObjectItem(root, "type");
                    int opcode = item->valueint;
                    if (opcode == ADD_FRIEND) // 添加好友
                    {
                        item = cJSON_GetObjectItem(root, "recv_id");
                        for (int j = 1; j <= user_counter; j++)
                        {
                            if (user[fds[j].fd].UID == item->valueint)
                            {
                                fds[j].events |= ~POLLIN;
                                fds[j].events |= POLLOUT;

                                // user[fds[j].fd].data = cJSON_CreateObject();
                                user[fds[j].fd].data = root;
                                break;
                            }
                            if (j == user_counter)
                            {
                                fds[i].events |= ~POLLIN;
                                fds[i].events |= POLLOUT;

                                user[connfd].data = cJSON_CreateObject();
                                cJSON_AddNumberToObject(user[connfd].data, "send_id", -1);
                                cJSON_AddStringToObject(user[connfd].data, "username", NULL);
                                cJSON_AddNumberToObject(user[connfd].data, "recv_id", user[connfd].UID);
                                cJSON_AddNumberToObject(user[connfd].data, "type", REPLY);
                                cJSON_AddStringToObject(user[connfd].data, "msg", "对方不在线，无法添加好友");
                            }
                        }
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