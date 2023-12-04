#include <iostream>

#include "Main_UI.h"
#include "./Friend.h"
using namespace std;

void Main_UI()
{
    // 主菜单
    cout << "===== 聊天室菜单 =====" << endl;
    cout << "1. 添加好友" << endl;
    cout << "2. 加入群聊" << endl;
    cout << "3. 私聊" << endl;
    cout << "4. 群聊" << endl;
    cout << "5. 查看好友" << endl;
    cout << "6. 删除好友" << endl;
    cout << "7. 退出群聊" << endl;
    cout << "8. 退出" << endl;
    cout << "=====================" << endl;
}

int Main_select(int sockfd, int uid, char *buf, int *status)
{
    int a = atoi(buf);

    cout << "Main_select " << uid << " " << buf << " " << status << endl;

    switch (a)
    {
    case 1:
        if (addFriend(sockfd, uid) == -1)
        {
            printf("Error:addFriend failed\n");
            return -1;
        }
        break;
    // case 2:
    //     joinGroupChat();
    //     break;
    // case 3:
    //     privateChat();
    //     break;
    // case 4:
    //     groupChat();
    //     break;
    case 5:
        viewFriends(uid);
        break;
    case 6:
        cout << "退出聊天室" << endl;
        break;
    default:
        cout << "无效选项，请重新输入" << endl;
        return -1;
        break;
    }
    return 0;
}
