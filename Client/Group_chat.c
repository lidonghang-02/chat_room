#include "./Config/config.h"
#include "./Group_chat.h"

int get_permission(int sockfd, char *username)
{
}

/**
 * @description: 判断是否是操作码
 * @param {char} *buf
 * @param {int} permissions
 * @return {*}
 */
int check_buf(char *buf, int permissions)
{
    if (strlen(buf) != 2 && buf[0] != '\\')
        return 0;
    int ret = 0;
    switch (permissions)
    {
    case OWNER:
        if (buf[1] - '0' > 1 && buf[1] - '0' < 9)
            ret = 1;
        break;
    case ADMIN:
        if (buf[1] - '0' > 1 && buf[1] - '0' <= 5)
            ret = 1;
        break;
    case MEMBER:
        if (buf[1] - '0' >= 1 && buf[1] - '0' <= 3)
            ret = 1;
        break;
    default:
        ret = -1;
    }
    return ret;
}

// 群主界面
void groupOwner_UI()
{
    printf("====== 群主界面 =======\n");
    printf("1. 解散群聊\t2. 设立管理员\n");
    printf("3. 踢人\t4. 修改群名称\n");
    printf("5. 发布群公告\t6. 转让群主\n");
    printf("7. 邀请\t8.退出群聊");
}

int groupOwner_select(int sockfd, char *username, int op)
{
    switch (op)
    {
    case 1:
    }
}

// 管理员界面
void groupAdmin_UI()
{
    printf("====== 管理员界面 ======\n");
    printf("1. 踢人\t2.发布群公告\n");
    printf("3. 退群\t4. 邀请\n");
    printf("5. 退出群聊\n");
}

// 群员界面
void groupMember_UI()
{
    printf("=== 群员界面 ===\n");
    printf("1. 退群\t2. 邀请");
    printf("3. 退出群聊\n");
}