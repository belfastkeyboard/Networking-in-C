#include <winsock2.h>
#include <stdio.h>

#define PORT 12345
#define NON_BLOCKING_MODE 1
#define LOCALHOST "127.0.0.1"
#define BUFFER_LEN 1024

typedef int STATUS;

void PrintWSAErrorMessage(int error);

STATUS CreateUDPSocket(SOCKET* sockfd);

void GetIPAddr(char* ipaddr);
