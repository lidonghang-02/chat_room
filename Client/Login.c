#include "./Config/config.h"
#include "./Login.h"

char *login(int sockfd)
{
    int ret = -1;
    char password[32];
    char username[32];

    printf("Please enter your username:");
    scnaf("%s", username);

    // 查询passwd是否正确
    printf("Please enter your password:");
    scanf("%s", password);

    cJSON *temp = cJSON_CreateObject();
    cJSON_AddNumberToObject(temp, "msgType", LOGIN);
    cJSON_AddStringToObject(temp, "username", username);
    cJSON_AddStringToObject(temp, "password", password);

    char *out = cJSON_Print(temp);
    if (send(sockfd, (void *)out, MSG_LEN, 0) < 0)
    {
        perror("ERROR: 请求服务器失败");
        exit(1);
    }
    return username;
}

char *sign_up(int sockfd)
{
    char username[32], password[32];
    printf("Please enter your username: ");
    scanf("%s", username);
    while (strlen(username) > 10)
    {
        printf("username too long\nPlease enter your username again\n");
        scanf("%s", username);
    }

    printf("Please enter your password:");
    scanf("%s", password);
    while (strlen(password) > 10)
    {

        print("password too long\nPlease enter your password again\n");
        scanf("%s", password);
    }

    cJSON *temp = cJSON_CreateObject();
    cJSON_AddNumberToObject(temp, "msgType", SIGN_UP);
    cJSON_AddStringToObject(temp, "username", username);
    cJSON_AddStringToObject(temp, "password", password);
    char *out = cJSON_Print(temp);
    if (send(sockfd, (void *)out, MSG_LEN, 0) < 0)
    {
        perror("ERROR: 请求服务器失败");
        exit(1);
    }
    return username;
}

/**
 * @description: 登陆界面
 * @param {int} sockfd
 * @return {0：退出，1：登陆成功，-1：登陆失败}
 */
int Login_UI(int sockfd, char *username)
{
    int opt;
    int id = -1;

    printf("--------------------------------\n");
    printf("--- Welcome to the chat room ---\n");
    printf("--------------------------------\n\n");

    while (1)
    {
        printf("--- 1.login	2.sign up 3.exit----\n");
        printf("\nPlease enter options:");
        scanf("%d", &opt);
        switch (opt)
        {
        case 1:
            username = login(sockfd);
            printf("login success\n");
            return 1;

            break;
        case 2:

            username = sign_up(sockfd);
            printf("--------------------------\n");
            printf("user[%s] create success\n", username);
            printf("--------------------------\n");
            break;

        case 3:
            printf("exit success\n");
            return 0;

        default:
            printf("\nPlease enter the correct options!\n");
        }
    }
}