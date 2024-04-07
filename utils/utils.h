#ifndef NETWORKING_UTILITIES
#define NETWORKING_UTILITIES 1

#define _WIN32_WINNT _WIN32_WINNT_VISTA

#include <winsock2.h>
#include <ws2tcpip.h>

#include "status.h"

#define PORT 54000
#define BUFFER_LEN 4096
#define WSA_SUCCESS 0

typedef int STATUS;

void PrintWSAErrorMessage(int error);

STATUS NetworkWSAInitialisation(void);
STATUS NetworkWSACleanup(void);

STATUS NetworkCreateSocket(SOCKET* sockfd, int af, int type, int protocol);
STATUS NetworkCloseSocket(SOCKET sockfd);
STATUS NetworkBindSocket(SOCKET sockfd, struct sockaddr_in* addr, int addrlen);
STATUS NetworkListenSocket(SOCKET sockfd, int backlog);
STATUS NetworkWaitForConnection(struct sockaddr_in* addr, SOCKET serv, SOCKET* recv);

void NetworkConstructSockaddr_in(struct sockaddr_in* addr, short fam, u_short port, u_long S_addr);

#endif 
