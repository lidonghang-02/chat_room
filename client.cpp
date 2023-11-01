/*
 * @Date: 2023-10-22 13:17:39
 * @author: lidonghang-02 2426971102@qq.com
 * @LastEditTime: 2023-11-01 10:30:58
 */
#include <iostream>
#include <fcntl.h>
#include "chat_room.h"

#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <poll.h>
#include <unistd.h>

using namespace std;

int UID, FID;
char username[15];

int connect(const char *ip, int port);
void chat(int sockfd, int opcode, char *name, int FID);

int main(int argc, char *argv[])
{
	if (argc <= 2)
	{
		fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
		exit(1);
	}
	int opt, ret;
	const char *ip = argv[1];
	int port = atoi(argv[2]);
	int sockfd;
	if ((sockfd = connect(ip, port)) == -1)
	{
		cout << "connect server failed" << endl;
		exit(1);
	}

	if (Init_SQL() == -1)
	{
		cout << "sql init error" << endl;
		// exit(1);
	}
stp1:
	cout << "1.login	2.sign up" << endl;
	cin >> opt;
	getchar();
	switch (opt)
	{
	case 1:
		if (login(username, UID) == -1)
		{
			cout << "login failed" << endl;
			goto stp1;
		}
		MSG msg;
		strcpy(msg.name, username);
		msg.UID = UID;
		msg.opcode = LOGIN;

		if (send(sockfd, &msg, sizeof(msg), 0) == -1)
		{
			printf("send user data error\n");
		}

		break;
	case 2:
		if (create_user(username, UID) == -1)
		{
			cout << "create user failed" << endl;
		}
		goto stp1;
		break;
	default:
		cout << "Please enter the correct opcode" << endl;
		goto stp1;
	}

stp2:
	cout << "------------------------------------------------------" << endl;
	cout << "1.add friend   2.public chat   3.private chat   4.quit" << endl;
	cout << "------------------------------------------------------" << endl;

	cin >> opt;
	getchar();
	switch (opt)
	{
	case 1:
		printf("Please enter your friend's ID:");
		cin >> FID;
		if (FID == UID)
		{
			printf("The %d is yourself.\n", FID);
			goto stp2;
		}
		ret = check_user_exists(FID);
		if (ret == 0)
		{
			cout << "user[" << FID << "] does not exist" << endl;
			goto stp2;
		}
		// MSG msg;
		// msg.UID = UID;
		// msg.FID = FID;
		// msg.opcode = Add_Friend;
		// strcpy(msg.name, username);
		// if (send(sockfd, &msg, sizeof(msg), 0) == -1)
		// {
		// 	printf("send Friend verification error\n");
		// }
		// printf("Wait for user(%d) consent...\n", FID);
		// recv(sockfd, &msg, sizeof(msg), MSG_WAITALL);
		// if (strcmp(msg.buf, "agree") == 0)
		// {
		if (add_friend(UID, FID) == -1)
		{
			cout << "Failed to add friend" << endl;
		}
		// }
		// else if (strcmp(msg.buf, "reject") == 0)
		// {
		// 	cout << "The other party rejected your friend application" << endl;
		// }
		goto stp2;

		break;
	case 2:
		printf("-------------------------------\n");
		printf("----------public chat----------\n");
		printf("------ Type quit to exit ------\n");
		printf("-------------------------------\n");

		chat(sockfd, Public_Chat, username, 0);
		goto stp2;
		break;
	case 3:
		cout << "Please enter your firend's UID" << endl;
		cin >> FID;
		if (FID == UID)
		{
			printf("The %d is yourself.\n", FID);
			goto stp2;
		}
		if (is_friend(UID, FID) != 0)
		{
			goto stp2;
		}
		ret = get_friend_state(FID);
		if (ret == NOT_ONLINE)
		{
			cout << "Your friend is not online" << endl;
			goto stp2;
		}
		printf("--------------------------------\n");
		printf("------private chat with %d------\n", FID);
		printf("------ Type quit to exit -------\n");
		printf("--------------------------------\n");

		chat(sockfd, Private_Chat, username, FID);
		goto stp2;
		break;
	case 4:
		user_quit(UID);
		break;
	default:
		cout << "Please enter the correct opcode" << endl;
		goto stp2;
		break;
	}

	return 0;
}

int connect(const char *ip, int port)
{
	pollfd fds[2];
	MSG msg;
	// 服务器地址
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;			   // ipv4
	server_addr.sin_port = htons(port);			   // port
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

void chat(int sockfd, int opcode, char *name, int FID)
{

	pollfd fds[2];
	int ret;
	MSG msg_send, msg_recv;

	msg_recv.opcode = opcode;
	msg_send.opcode = opcode;
	strcpy(msg_recv.name, name);
	strcpy(msg_send.name, name);
	msg_send.UID = UID;
	msg_send.FID = FID;

	memset(msg_send.buf, '\0', BUFFER_SIZE);
	if (send(sockfd, &msg_send, sizeof(msg_send), 0) == -1)
	{
		printf("send text error\n");
	}

	// fds[0]接受用户输入数据
	// fds[1]监视服务器状态
	fds[0].fd = 0;
	fds[0].events = POLLIN;
	fds[0].revents = 0;
	fds[1].fd = sockfd;
	fds[1].events = POLLIN | POLLRDHUP;
	fds[1].revents = 0;

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
			memset(msg_recv.buf, '\0', BUFFER_SIZE);
			recv(fds[1].fd, &msg_recv, sizeof(msg_recv), 0);
			cout << msg_recv.buf << endl;
			/*
			// 响应验证消息
			if (msg_recv.opcode == Add_Friend)
			{
				MSG msg;
				msg.UID = UID;
				msg.FID = msg_recv.FID;
				strcpy(msg.name, name);

				int opt;
				cout << "1.agree		2.reject" << endl;
				while (cin >> opt)
				{
					if (opt == 1)
					{
						strcpy(msg.buf, "agree");
						break;
					}
					else if (opt == 2)
					{
						strcpy(msg.buf, "reject");
						break;
					}
					else
						printf("Please enter the correct opcode\n");
				}
				if (send(sockfd, &msg, sizeof(msg), 0) == -1)
				{
					printf("Failed to send verification message\n");
				}
			}
			*/
		}

		if (fds[0].revents & POLLIN)
		{
			fgets(msg_send.buf, BUFFER_SIZE, stdin);
			msg_send.buf[strlen(msg_send.buf) - 1] = '\0';

			// 退出操作
			if (strcmp(msg_send.buf, "quit") == 0)
			{
				return;
			}
			if (send(sockfd, &msg_send, sizeof(msg_send), 0) == -1)
			{
				printf("send text error\n");
			}
		}
	}
}