#include <iostream>
#include <arpa/inet.h>

#include "../Common/msg_type.h"
#include "../database/sql_func.h"
#include "./Friend.h"
#include "../cJSON/cJSON.h"

using namespace std;

int addFriend(int sockfd, int id)
{
    int fid;
    char msg[128];
    printf("输入好友id：");
    cin >> fid;
    if (check_user_exists(fid) <= 0)
    {
        return -1;
    }

    cJSON *verify = cJSON_CreateObject();
    cJSON_AddNumberToObject(verify, "send_id", id);
    cJSON_AddStringToObject(verify, "username", get_username(id));
    cJSON_AddNumberToObject(verify, "recv_id", fid);
    cJSON_AddNumberToObject(verify, "type", ADD_FRIEND);
    sprintf(msg, "%s(%d)请求添加你为好友", get_username(id), id);
    cJSON_AddStringToObject(verify, "msg", msg);
    char *out = cJSON_Print(verify);
    if (send(sockfd, (void *)out, MSG_LEN, 0) < 0)
    {
        perror("ERROR: 请求服务器失败");
        return -1;
    }
    return 0;
}

int viewFriends(int uid)
{
    get_friend_list(uid);
}