#ifndef GROUP_CHAT_H
#define GROUP_CHAT_H

int check_buf(char *buf, int permissions);
void groupOwner_UI();
int groupOwner_select(int sockfd, char *username, int op);
void groupAdmin_UI();
void groupMember_UI();

#endif // GROUP_CHAT_H