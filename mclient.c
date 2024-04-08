#include "utils/utils.h"

#define IP_ADDRESS "127.0.0.1"

int main(void)
{

    WSADATA data;
    SOCKET sockfd;
    struct sockaddr_in addr;

    // Begin winsock
    if (NetworkWSAInitialisation(&data))
        return EXIT_FAILURE;

    // Create socket
    if (NetworkCreateSocket(&sockfd, AF_INET, SOCK_STREAM, IPPROTO_IP))
        return EXIT_FAILURE;

    // Construct addr_in struct
    NetworkConstructSockaddr_in(&addr, AF_INET, PORT, inet_addr(IP_ADDRESS));
    
    // Connect to a server
    if (NetworkConnect(sockfd, &addr))
        return EXIT_FAILURE;

    // how can client recv / send simultaneously?

    if (NetworkClientSend(sockfd))
        return EXIT_FAILURE;

    // Shut down
    if (NetworkCloseSocket(sockfd))
        return EXIT_FAILURE;

    if (NetworkWSACleanup())
        return EXIT_FAILURE;

    return 0;
}