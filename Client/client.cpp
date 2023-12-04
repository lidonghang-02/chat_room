/*
 * @Date: 2023-11-06 08:51:26
 * @author: lidonghang-02 2426971102@qq.com
 * @LastEditTime: 2023-12-04 18:09:47
 */
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <poll.h>
#include <arpa/inet.h>

#include "../database/sql_func.h"
#include "../Common/msg_type.h"
#include "../cJSON/cJSON.h"
#include "Login_UI.h"
#include "Main_UI.h"

using namespace std;

int connect(const char *ip, int port);

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
        cout << "Error: open sql failed." << endl;
        exit(1);
    }
    else
    {
        cout << "open sql successfully" << endl;
    }

    // 参数
    char buf[64];
    int sockfd;
    int status;
    int ret;
    int uid;
    pollfd fds[2];

    // 连接服务器
    if (sockfd = connect(argv[1], atoi(argv[2])) == -1)
    {
        cout << "Error: connect server failed" << endl;
        exit(1);
    }

    // 初始化参数
    // fds[0]接受用户输入数据
    // fds[1]监视服务器状态
    fds[0].fd = 0;
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    fds[1].fd = sockfd;
    fds[1].events = POLLIN | POLLRDHUP;
    fds[1].revents = 0;

    // 登陆界面
    ret = Login_UI();
    if (ret == -1)
    {
        printf("ERROR: main_ui error\n");
        exit(1);
    }
    Main_UI();
    status = MAIN_UI;
    while (1)
    {
        ret = poll(fds, 2, -1);
        if (ret < 0)
        {
            printf("poll failure\n");
            break;
        }
        if (fds[1].revents & POLLRDHUP)
        {
            printf("server close the connection");
            break;
        }
        else if (fds[1].revents & POLLIN)
        {
            char msg[MSG_LEN];
            recv(fds[1].fd, msg, MSG_LEN, 0);
            cJSON *root = cJSON_Parse(msg);
            cJSON *item = cJSON_GetObjectItem(root, "type");
            int opcode = item->valueint;
            switch (opcode)
            {
            case ADD_FRIEND:
                break;
            }
        }

        if (fds[0].revents & POLLIN)
        {
            fgets(buf, sizeof(buf), stdin);
            switch (status)
            {
            case MAIN_UI:
                ret = Main_select(sockfd, uid, buf, status);
                if (ret == -1)
                {
                    Main_UI();
                }
                break;

            default:
                break;
            }
        }
    }
}

int connect(const char *ip, int port)
{
    pollfd fds[2];
    // 服务器地址
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;              // ipv4
    server_addr.sin_port = htons(port);            // port
    inet_pton(AF_INET, ip, &server_addr.sin_addr); // ip

    int sockfd = socket(AF_INET, SOCK_STREAM, 0); // ipv4 TCP
    if (sockfd == -1)
    {
        cout << "socket error" << endl;
        return -1;
    }

    // 向服务器发送连接请求
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        cout << "connect error" << endl;
        return -1;
    }
    return sockfd;
}
