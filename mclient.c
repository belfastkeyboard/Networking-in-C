#include "utils/utils.h"

#define IP_ADDRESS LOCALHOST

int main(void)
{

    WSADATA data;
    SOCKET sockfd;
    struct sockaddr_in addr;

    if (NetworkWSAInitialisation(&data))
        return EXIT_FAILURE;

    if (NetworkCreateSocket(&sockfd, AF_INET, SOCK_STREAM, IPPROTO_IP))
        return EXIT_FAILURE;

    NetworkConstructSockaddr_in(&addr, AF_INET, PORT, inet_addr(IP_ADDRESS));
    
    if (NetworkConnect(sockfd, &addr))
        return EXIT_FAILURE;

    // requires better thread handling
    NetworkClientSendReceive(sockfd); // always exits with EXIT_SUCCESS

    if (NetworkCloseSocket(sockfd))
        return EXIT_FAILURE;

    if (NetworkWSACleanup())
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}