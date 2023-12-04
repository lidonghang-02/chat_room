/*
 * @Date: 2023-11-06 13:44:25
 * @author: lidonghang-02 2426971102@qq.com
 * @LastEditTime: 2023-11-15 16:09:37
 */
#include <iostream>
#include <string.h>
#include "../database/sql_func.h"

using namespace std;

int login()
{
    int UID, ret = -1;
    char password[32];

    // 查询UID是否正确
    printf("Please enter your UID:");
    cin >> UID;
    if (check_user_exists(UID) <= 0)
    {
        return -1;
    }

    // 查询passwd是否正确
    printf("Please enter your password:");
    cin >> password;
    if (check_password(UID, password) <= 0)
    {
        return -1;
    }

    ret = get_user_status(UID);
    if (ret == -1)
    {
        return -1;
    }
    else if (ret != Offline)
    {
        printf("ERROE: User has login in\n");
        return -1;
    }
    else // 修改用户状态为在线
    {
        if (update_user_status(UID, Online) == -1)
            return -1;
    }

    return 0;
}

int sign_up()
{
    char username[32], password[32];
    printf("Please enter your username: ");
    scanf("%s", username);
    while (strlen(username) > 10)
    {
        cout << "username too long" << endl;
        cout << "Please enter your username again" << endl;
        scanf("%s", username);
    }

    printf("Please enter your password:");
    while (scanf("%s", password))
    {
        if (strlen(password) > 10)
        {
            cout << "password too long" << endl;
            cout << "Please enter your password again" << endl;
        }
        else
            break;
    }

    int UID = create_user(username, password);
}

// 主界面
int Login_UI()
{
    int opt;
    int id = -1;

    printf("--------------------------------\n");
    printf("--- Welcome to the chat room ---\n");
    printf("--------------------------------\n\n");

    while (1)
    {

        printf("------ 1.login	2.sign up ------\n");
        printf("\nPlease enter options:");
        cin >> opt;
        switch (opt)
        {
        case 1:
            cout << "login" << endl;
            if (login() == -1)
            {
                printf("ERROR: Login failed\n");
                return -1;
            }
            else
            {
                printf("login success\n");
                return 1;
            }
            break;
        case 2:
            cout << "signup" << endl;
            if ((id = sign_up()) == -1)
            {
                printf("ERROR: Signup failed\n");
                return -1;
            }
            else
            {
                printf("--------------------------\n");
                printf("user[%d] create success\n", id);
                printf("your UID is %d\n", id);
                printf("The UID is used to login\n");
                printf("--------------------------\n");
            }
            break;

        default:
            printf("\nPlease enter the correct options!\n");
            break;
        }
    }
    return 1;
}