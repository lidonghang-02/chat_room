// TCP网络编程之客户端

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include "./cJSON/cJSON.h"
#include "../Common/msg_type.h"

#define N 128
#define ERRLOG(errmsg)                                          \
    do                                                          \
    {                                                           \
        perror(errmsg);                                         \
        printf("%s - %s - %d\n", __FILE__, __func__, __LINE__); \
        exit(1);                                                \
    } while (0)

int main(int argc, char const *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }

    int sockfd;
    struct sockaddr_in serveraddr;
    socklen_t addrlen = sizeof(serveraddr);

    // 第一步：创建套接字
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        ERRLOG("socket error");
    }

    // 第二步：填充服务器网络信息结构体
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
    serveraddr.sin_port = htons(atoi(argv[2]));

    // 第三步：给服务器发送客户端的连接请求
    if (connect(sockfd, (struct sockaddr *)&serveraddr, addrlen) == -1)
    {
        ERRLOG("connect error");
    }

    // 进行通信
    char buf[1024] = {0};
    while (1)
    {
        int id = 1, fid = 2;

        cJSON *verify = cJSON_CreateObject();
        cJSON_AddNumberToObject(verify, "send_id", id);
        cJSON_AddStringToObject(verify, "username", "sss");
        cJSON_AddNumberToObject(verify, "recv_id", fid);
        cJSON_AddNumberToObject(verify, "type", ADD_FRIEND);
        cJSON_AddStringToObject(verify, "msg", "");
        char *out = cJSON_Print(verify);
        // printf("%s\n", out);
        if (send(sockfd, (void *)out, 1024, 0) < 0)
        {
            perror("ERROR: 请求服务器失败");
            return -1;
        }
        if (recv(sockfd, buf, 1024, 0) == -1)
        {
            ERRLOG("recv error");
        }

        printf("服务器：%s\n", buf);
    }

    return 0;
}
