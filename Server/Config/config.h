// #ifndef CONFIG_H_
// #define CONFIG_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>
#include <string.h>

#include "../cJSON/cJSON.h"
#include "../database/sql_func.h"

#define MSG_LEN 1024
#define USER_MAX 32
// msg类型
enum toServerMsgType
{

    LOGIN,               /*登陆请求*/
    SIGN_UP,             /*注册请求*/
    MAIN_UI,             /*主界面*/
    EXIT,                /*退出请求*/
    ADD_FRIEND,          /*添加好友*/
    JOIN_GROUP,          /*加入群聊*/
    GROUP_CHAT,          /*群聊请求*/
    PERSONAL_CHAT,       /*私聊请求*/
    PROCESS_APPLICATION, /*处理申请*/
    VIEW_FRIENDS_LIST,   /*查看好友列表*/
    VIEW_GROUPS_LIST,    /*查看群聊列表*/
    VIEW_RECORDS,        /*查看聊天记录请求*/
    VIEW_INFORMATION,    /*查看帐号信息*/
    DELETE_FRIEND,       /*删除好友*/
    QUIT_GROUP,          /*退出群聊*/
    UPDATE_INFORMATION,  /*修改帐号信息*/
    INSERT_GROUPLIST,    /*加入群聊列表*/
    DELETE_GROUPLIST,    /*退出群聊列表*/
    FRIEND_VERIFY_REPLY, /*好友请求验证回复*/
    GROUP_VERIFY_REPLY,  /*群聊验证回复*/
    AGREE,               /*同意*/
    REJECT,              /*拒绝*/
    JUMP_OVER,           /*跳过*/
    RE_ENTER,            /*重新输入请求*/
    SELECT_PERSON,       /*查询私聊请求*/
    SELECT_GROUP,        /*查询群聊请求*/
    UNKONWN              /*未知请求类型*/
};
enum toClientMsgType
{
    ERROR = 1,
    SUCCESS,
    RESULT, /*结果消息类型*/
    VERIFY
};
// 数据库申请信息表（Application）和消息表（Message）的类型
enum Table
{
    MES_PERSONAL = 1,
    MES_GROUP,
    APPLY_FRIEND,
    APPLY_GROUP
};
// 群聊成员权限等级
enum Permissions
{
    OWNER, // 群主
    ADMIN, // 管理员
    MEMBER // 群成员
};
struct userdate
{
    char username[32];
    char buf[MSG_LEN];
    cJSON *data;
};
typedef struct _MemberNode
{
    char username[32];
    int pos;
    struct _MemberNode *next;
} MemberNode;
typedef struct _GroupNode
{
    char groupname[32];
    MemberNode *GroupMembers;
    struct _GroupNode *next;
} GroupNode;

// #endif /* CONFIG_H_ */