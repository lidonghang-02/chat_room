/*
 * @Date: 2023-12-12 13:22:19
 * @author: lidonghang-02 2426971102@qq.com
 * @LastEditTime: 2023-12-19 18:56:10
 */
#include "./Config/config.h"
#include "./Login.h"
#include "./Main_UI.h"

int connect_server(const char *ip, int port);
void work_application(char *application, int *pos, char *send_name, int *type, char *groupname);
void update_information();

int main(int argc, char *argv[])
{
    if (argc <= 2)
    {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }

    // 参数
    char buf[64];          // 输入信息
    char username[32];     // 用户名
    char target[32];       // 聊天目标
    char application[512]; // 存储申请信息
    int pos = 0;           // 存储申请信息的位置
    int sockfd;            // socket套接字描述符
    int status;            // 当前状态
    int ret;               // 函数返回值
    int opcode;            // server端发来的消息类型
    struct pollfd fds[2];

    cJSON *root = NULL, *item = NULL;

    // 连接服务器
    if (sockfd = connect_server(argv[1], atoi(argv[2])) == -1)
    {
        printf("Error: connect server failed");
        exit(1);
    }

    // 初始化参数
    // fds[0]接受用户输入数据
    // fds[1]监视服务器状态
    fds[0].fd = 0;
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    fds[1].fd = sockfd;
    fds[1].events = POLLIN | POLLHUP;
    fds[1].revents = 0;

    ret = Login_UI(sockfd, username);
    if (ret == 0 || username == NULL)
    {
        printf("Login failed");
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
        if (fds[1].revents & POLLHUP)
        {
            printf("server close the connection");
            break;
        }
        else if (fds[1].revents & POLLIN)
        {
            char temp[MSG_LEN];
            recv(fds[1].fd, temp, MSG_LEN, 0);
            root = cJSON_Parse(temp);
            item = cJSON_GetObjectItem(root, "msgType");
            int opcode = item->valueint;
            switch (opcode)
            {
                if (opcode == VERIFY && status == PROCESS_APPLICATION)
                {
                    item = cJSON_GetObjectItem(root, "msg");
                    strcpy(application, item->valuestring);
                }
            }
        }

        if (fds[0].revents & POLLIN)
        {
            fgets(buf, sizeof(buf), stdin);
            if (status == MAIN_UI)
            {
                status = Main_select(sockfd, buf, username, target);
                while (status == RE_ENTER)
                {
                    system("clear");
                    Main_UI();
                    status = Main_select(sockfd, buf, username, target);
                }
            }
            else if (status == PERSONAL_CHAT)
            {
                cJSON *temp = cJSON_CreateObject();
                cJSON_AddNumberToObject(temp, "msgType", PERSONAL_CHAT);
                cJSON_AddStringToObject(temp, "username", username);
                cJSON_AddStringToObject(temp, "personname", target);
                cJSON_AddStringToObject(temp, "msg", buf);
                char *out = cJSON_Print(temp);
                if (send(sockfd, (void *)out, MSG_LEN, 0) < 0)
                {
                    perror("ERROR: 请求服务器失败");
                    exit(1);
                }
            }
            else if (status == GROUP_CHAT)
            {
                cJSON *temp = cJSON_CreateObject();
                cJSON_AddNumberToObject(temp, "msgType", GROUP_CHAT);
                cJSON_AddStringToObject(temp, "username", username);
                cJSON_AddStringToObject(temp, "groupname", target);
                cJSON_AddStringToObject(temp, "msg", buf);
                char *out = cJSON_Print(temp);
                if (send(sockfd, (void *)out, MSG_LEN, 0) < 0)
                {
                    perror("ERROR: 请求服务器失败");
                    exit(1);
                }
            }
            else if (status == PROCESS_APPLICATION)
            {
                int type = 0;
                char send_name[32], groupname[32];
                cJSON *temp = cJSON_CreateObject();

                work_application(application, &pos, send_name, &type, groupname);

                if (type == APPLY_FRIEND)
                    cJSON_AddNumberToObject(temp, "msgType", FRIEND_VERIFY_REPLY);
                else if (type == APPLY_GROUP)
                {
                    cJSON_AddNumberToObject(temp, "msgType", GROUP_VERIFY_REPLY);
                    cJSON_AddStringToObject(temp, "grouname", groupname);
                }
                cJSON_AddStringToObject(temp, "username", username);
                cJSON_AddStringToObject(temp, "target", send_name);
                int choice = atoi(buf);
                switch (choice)
                {
                case 1:
                    cJSON_AddNumberToObject(temp, "result", AGREE);
                    break;
                case 2:
                    cJSON_AddNumberToObject(temp, "result", REJECT);
                    break;
                case 3:
                    cJSON_AddNumberToObject(temp, "result", JUMP_OVER);
                    break;
                default:
                    printf("Unknown choice %d\n", choice);
                    goto stp;
                }
                // 向服务器发送结果
                char *out = cJSON_Print(temp);
                if (send(sockfd, (void *)out, MSG_LEN, 0) < 0)
                {
                    perror("ERROR: 请求服务器失败");
                    exit(1);
                }
            stp:
            }
            else if (status == UPDATE_INFORMATION)
            {
                printf("8.退出");
                int stop = 1;
                char update[8];
                memset(update, 0, sizeof(update));
                cJSON *temp = cJSON_CreateObjec();
                cJSON_AddNumberToObject(temp, "msgType", UPDATE_INFORMATION);
                cJSON_AddStringToObject(temp, "username", username);
                while (stop)
                {
                    printf("Please enter the item number to be modified:\n");
                    int choice = 0;
                    scanf("%d", &choice);
                    switch (choice)
                    {
                    case 2:
                        printf("Please enter the new password\n");
                        char new_password[12];
                        scanf("%s", new_password);
                        cJSON_AddStringToObject(temp, "password", new_password);
                        strcat(update, "2");
                        break;
                    case 3:
                        printf("Please enter the new email\n");
                        char new_email[32];
                        scanf("%s", new_email);
                        cJSON_AddStringToObject(temp, "email", new_email);
                        strcat(update, "3");
                        break;
                    case 4:
                        printf("Please enter the new status\n");
                        int new_status = 0;
                        scanf("%d", &new_status);
                        cJSON_AddNumberToObject(temp, "status", new_status);
                        strcat(update, "4");
                        break;
                    case 5:
                        printf("Please enter the new profile\n");
                        char new_profile[128];
                        scanf("%s", new_profile);
                        cJSON_AddStringToObject(temp, "profile", new_profile);
                        strcat(update, "5");
                        break;
                    case 8:
                        stop = 0;
                        break;
                    default:
                        printf("ERROR:Please enter the correct opcode\n");
                        break;
                    }
                }
                cJSON_AddStringToObject(temp, "update", update);
                char *out = cJSON_Print(temp);
                if (send(sockfd, (void *)out, MSG_LEN, 0) < 0)
                {
                    perror("ERROR: 请求服务器失败");
                    exit(1);
                }
            }
        }
    }
    return 0;
}
char *substr(const char *str, int start, int length)
{
    int str_length = strlen(str);
    if (start >= str_length)
    {
        return NULL; // 起始位置超出字符串长度，返回 NULL
    }
    if (start + length > str_length)
    {
        length = str_length - start; // 截取长度超过字符串剩余长度，截取剩余部分
    }
    char *substr = (char *)malloc((length + 1) * sizeof(char));
    strncpy(substr, str + start, length);
    substr[length] = '\0'; // 添加字符串结尾的空字符
    return substr;
}

int connect_server(const char *ip, int port)
{
    struct pollfd fds[2];
    // 服务器地址
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;              // ipv4
    server_addr.sin_port = htons(port);            // port
    inet_pton(AF_INET, ip, &server_addr.sin_addr); // ip

    int sockfd = socket(AF_INET, SOCK_STREAM, 0); // ipv4 TCP
    if (sockfd == -1)
    {
        perror("socket");
        return -1;
    }

    // 向服务器发送连接请求
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("connect");
        return -1;
    }
    return sockfd;
}
/**
 * @description: 处理申请信息
 * @param {char} *application 申请信息
 * @param {int} *pos 字符串位置
 * @param {char} *send_name 获取发送者username
 * @param {int} *type 获取信息类型（APPLY_FRIEND/APPLY_GROUP)
 */
void work_application(char *application, int *pos, char *send_name, int *type, char *groupname)
{
    //"#%send_name$$%msg_type($$groupname)$$%content----%send_at#"
    int i = (*pos), j = 0;
    while (1)
    {
        if (application[i] == '#')
        {
            j = i;
            while (1)
            {
                if (application[i] == '$' && application[i + 1] == '$')
                {
                    strncpy(send_name, application + j + 1, i - j);
                    i += 2;
                    type = application[i] - '0';
                    i += 3;
                    if (type == APPLY_GROUP)
                    {
                        j = i;
                        while (application[i] != '$' || application[i + 1] != '$')
                        {
                            i++;
                        }
                        strncpy(groupname, application + j, i - j);
                    }
                    while (1)
                    {
                        if (application[i] == '#')
                        {
                            printf("1.agree\t2.reject\t3.jump over\n");
                            return;
                        }
                        printf("%c", application[i]);
                        i++;
                    }
                }
                printf("%c", application[i]);
                i++;
            }
        }
        i++;
    }
}
