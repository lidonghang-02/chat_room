#include "./Config/config.h"

void personal_chat(int sockfd, char *username)
{
    char personalChat_name[32];
    printf("Please enter the username of your personal chat target\n");
    scanf("%s", personalChat_name);
}