/*
 * @Date: 2023-10-24 10:03:02
 * @author: lidonghang-02 2426971102@qq.com
 * @LastEditTime: 2023-10-30 17:53:17
 */
#include <iostream>
#include <netinet/in.h>
#include <poll.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include "chat_room.h"
using namespace std;

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
    assert(listenfd >= 0);

    ret = bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    assert(ret != -1);

    ret = listen(listenfd, 5);
    assert(ret != -1);

    MSG *user = new MSG[FD_SETSIZE];
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
            printf("poll failure\n");
            break;
        }
        else if (ret == 0 && user_counter == 0)
        {
            printf("timeout\n");
            break;
        }
        for (int i = 0; i < user_counter + 1; i++)
        {
            if ((fds[i].fd == listenfd) && (fds[i].revents & POLLIN))
            {
                if (user_counter >= USER_MAX)
                {
                    printf("too many users\n");
                    continue;
                }
                struct sockaddr_in client_addr;
                socklen_t client_addrLen = sizeof(client_addr);
                int connfd = accept(listenfd, (struct sockaddr *)&client_addr, &client_addrLen);
                if (connfd < 0)
                {
                    printf("errno is : %d\n", errno);
                    continue;
                }

                user_counter++;
                fds[user_counter].fd = connfd;
                fds[user_counter].events = POLLIN | POLLRDHUP | POLLERR;
                fds[user_counter].revents = 0;
                printf("comes a new user, now have %d user\n", user_counter);
            }
            else if (fds[i].revents & POLLERR) // 出错
            {
                printf("get an error from %d\n", fds[i].fd);
                char errors[100];
                memset(errors, '\0', 100);
                socklen_t len = sizeof(errors);
                // 获取套接字的错误状态
                if (getsockopt(fds[i].fd, SOL_SOCKET, SO_ERROR, &errors, &len) < 0)
                {
                    printf("get socket option failed\n");
                }
                continue;
            }
            else if (fds[i].revents & POLLHUP)
            {
                printf("uer[%s](%d) left\n", user[fds[i].fd].name, user[fds[i].fd].UID);
                // 如果客户端关闭连接，服务器也关闭对应的连接，并将用户总数-1
                user[fds[i].fd] = {};
                close(fds[i].fd);
                fds[i] = fds[user_counter]; // poll监听数组也要更改
                user_counter--;
                i--; // 删除了一个，i要往前面走一个
            }
            else if (fds[i].revents & POLLIN) // 有数据可读
            {
                MSG msg;
                int connfd = fds[i].fd;
                ret = recv(connfd, &msg, sizeof(msg), 0);
                // 更新状态
                // if (user[connfd].opcode == LOGIN)
                // {
                // user[connfd].UID = msg.UID;
                // strcpy(user[connfd].name, msg.name);
                user[connfd].opcode = msg.opcode;
                // continue;
                // }
                if (ret < 0) // 如果读出错，关闭连接
                {
                    if (errno != EAGAIN)
                    {
                        close(connfd);
                        user[fds[i].fd] = {};
                        fds[i] = fds[user_counter];
                        user_counter--;
                        i--;
                    }
                }
                else
                {
                    // 连接已经断开
                    if (ret == 0)
                    {

                        printf("uer[%s](%d) left\n", user[connfd].name, user[connfd].UID);
                        // 关闭连接
                        close(connfd);
                        user[connfd] = {};
                        fds[i] = fds[user_counter];
                        user_counter--;
                        i--;
                    }
                    else // 接收到客户数据，通知其他socket连接准备写数据
                    {
                        int len;
                        char buf_temp[64];
                        memset(buf_temp, '\0', 64);
                        if (msg.opcode == Private_Chat)
                        {
                            len = snprintf(NULL, 0, "private chat from %s(%d): %s", msg.name, msg.UID, msg.buf);
                            snprintf(buf_temp, len + 1, "private chat from %s(%d): %s", msg.name, msg.UID, msg.buf);

                            for (int j = 1; j <= user_counter; ++j)
                            {
                                if (user[fds[j].fd].UID == msg.FID)
                                {
                                    if (user[fds[j].fd].opcode != Private_Chat)
                                    {

                                        len = snprintf(NULL, 0, "Received a private message from %s(%d): %s", msg.name, msg.UID, msg.buf);
                                        snprintf(buf_temp, len + 1, "Received a private message from %s(%d): %s", msg.name, msg.UID, msg.buf);
                                    }

                                    fds[j].events |= ~POLLIN; // 移除POLLIN（可读）事件
                                    fds[j].events |= POLLOUT; // 添加POLLOUT（可写）事件
                                    strcpy(user[fds[j].fd].buf, buf_temp);
                                    break;
                                }
                            }
                        }
                        else if (msg.opcode == Add_Friend)
                        {

                            len = snprintf(NULL, 0, "Verified by a friend from %s(%d)", msg.name, msg.UID);
                            snprintf(buf_temp, len + 1, "Verified by a friend from %s(%d)", msg.name, msg.UID);

                            for (int j = 1; j <= user_counter; ++j)
                            {
                                if (user[fds[j].fd].UID == msg.FID)
                                {
                                    fds[j].events |= ~POLLIN; // 移除POLLIN（可读）事件
                                    fds[j].events |= POLLOUT; // 添加POLLOUT（可写）事件
                                    strcpy(user[fds[j].fd].buf, buf_temp);

                                    user[fds[j].fd].opcode = Add_Friend;
                                    user[fds[j].fd].FID = msg.FID;

                                    cout << "message: " << user[fds[j].fd].opcode << "  " << user[fds[j].fd].UID << "  " << user[fds[j].fd].FID << endl;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            if (msg.opcode == LOGIN) // 登陆，存储用户的UID和name
                            {
                                user[connfd].UID = msg.UID;
                                strcpy(user[connfd].name, msg.name);
                                user[connfd].opcode = msg.opcode;

                                len = snprintf(NULL, 0, "----%s(%d)上线----", msg.name, msg.UID);
                                snprintf(buf_temp, len + 1, "----%s(%d)上线----", msg.name, msg.UID);
                            }
                            else if (msg.opcode == Public_Chat)
                            {

                                len = snprintf(NULL, 0, "public chat from %s(%d): %s", msg.name, msg.UID, msg.buf);
                                snprintf(buf_temp, len + 1, "public chat from %s(%d): %s", msg.name, msg.UID, msg.buf);
                            }
                            for (int j = 1; j <= user_counter; ++j)
                            {
                                if (fds[j].fd == connfd || user[fds[j].fd].opcode != Public_Chat) // 当前的和不再个群聊的连接socket不处理
                                {
                                    continue;
                                }
                                fds[j].events |= ~POLLIN; // 移除POLLIN（可读）事件
                                fds[j].events |= POLLOUT; // 添加POLLOUT（可写）事件
                                strcpy(user[fds[j].fd].buf, buf_temp);
                            }
                        }
                        cout << "send data: " << buf_temp << endl;
                    }
                }
            }
            else if (fds[i].revents & POLLOUT)
            {
                int connfd = fds[i].fd;
                if (strcmp(user[connfd].buf, "") == 0) // 有数据可写
                {
                    continue;
                }
                ret = send(connfd, &user[connfd], sizeof(user[connfd]), 0);
                // 清空数据
                memset(user[connfd].buf, '\0', BUFFER_SIZE);
                // 写完数据需要重新注册fds[i]的可读事件
                fds[i].events |= ~POLLOUT;
                fds[i].events |= POLLIN;
            }
        }
    }
    delete[] user;
    close(listenfd);
    return 0;
}