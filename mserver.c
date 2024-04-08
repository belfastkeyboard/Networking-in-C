#include "utils/utils.h"


int main(void)
{
    WSADATA data;
    SOCKET sock_listening, sock_client;
    struct sockaddr_in addr;

    if (NetworkWSAInitialisation(&data))
        return EXIT_FAILURE;

    if (NetworkCreateSocket(&sock_listening, AF_INET, SOCK_STREAM, IPPROTO_IP))
        return EXIT_FAILURE;

    NetworkConstructSockaddr_in(&addr, AF_INET, PORT, INADDR_ANY);

    if (NetworkBindSocket(sock_listening, &addr))
        return EXIT_FAILURE;

    if (NetworkListenSocket(sock_listening, SOMAXCONN))
        return EXIT_FAILURE;

    if (NetworkMultiServerReceive(sock_listening))
        return EXIT_FAILURE;

    if (NetworkCloseSocket(sock_client))
        return EXIT_FAILURE;

    if (NetworkWSACleanup())
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
};