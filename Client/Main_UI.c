/*
 * @Date: 2023-12-13 13:42:09
 * @author: lidonghang-02 2426971102@qq.com
 * @LastEditTime: 2023-12-19 17:00:32
 */
#include "./Config/config.h"
#include "./func.h"
#include "Main_UI.h"

void Main_UI()
{
    // 主菜单
    printf("===== 聊天室菜单 =====\n");
    printf("1. 添加好友\n");
    printf("2. 加入群聊\n");
    printf("3. 私聊\n");
    printf("4. 群聊\n");
    printf("5. 处理申请\n");
    printf("6. 查看好友\n");
    printf("7. 查看群聊\n");
    printf("8. 查看聊天记录\n");
    printf("9. 修改帐号信息\n");
    printf("10. 退出聊天室\n");
    printf("=====================\n");
}

int Main_select(int sockfd, char *buf, char *username, char *target)
{
    int a = atoi(buf);
    int ret = MAIN_UI;
    switch (a)
    {
    case 1:
        ret = ADD_FRIEND;
        add_friend(sockfd, username);
        break;
    case 2:
        ret = JOIN_GROUP;
        join_group(sockfd, username);
        break;
    case 3:
        ret = PERSONAL_CHAT;
        target = get_personalChat_target(sockfd, username);
        if (strcmp(target, "N") == 0)
        {
            ret = RE_ENTER;
        }
        break;
    case 4:
        ret = GROUP_CHAT;
        target = get_groupChat_target(sockfd, username);
        if (strcmp(target, "N") == 0)
        {
            ret = RE_ENTER;
        }
        else
        {
            // 更新server的群聊列表
            inster_groupList(sockfd, username, target);
        }
        break;
    case 5:
        ret = PROCESS_APPLICATION;
        get_application(sockfd, username);
        break;
    case 6:
        ret = VIEW_FRIENDS_LIST;
        get_friendList(sockfd, username);
        break;
    case 7:
        ret = VIEW_GROUPS_LIST;
        get_groupList(sockfd, username);
        break;
    case 8:
        ret = VIEW_RECORDS;
        get_records(sockfd, username);
        break;
    case 9:
        ret = UPDATE_INFORMATION;
        get_informations(sockfd, username);

        break;
    case 10:
        ret = EXIT;
        break;
    default:
        ret = RE_ENTER;
        break;
    }
    return ret;
}
