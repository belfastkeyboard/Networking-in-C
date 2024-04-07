#include "utils/utils.h"

// select()

// FD_CLR - remove from set
// FD_SET - add to set
// FD_ZERO - clear to zero

int main(void)
{
    WSADATA data;
    SOCKET sock_listening, sock_client;
    struct sockaddr_in addr, addr_client;

    // Begin winsock
    if (NetworkWSAInitialisation(&data))
        return EXIT_FAILURE;

    // Create socket
    if (NetworkCreateSocket(&sock_listening, AF_INET, SOCK_STREAM, IPPROTO_IP))
        return EXIT_FAILURE;

    // Bind socket to IP addr and port
    NetworkConstructSockaddr_in(&addr, AF_INET, PORT, INADDR_ANY);
    if (NetworkBindSocket(sock_listening, &addr, sizeof(addr)))
        return EXIT_FAILURE;

    // Tell winsock the socket is listening
    if (NetworkListenSocket(sock_listening, SOMAXCONN))
        return EXIT_FAILURE;

    // Wait for connection
    if (NetworkWaitForConnection(&addr_client, sock_listening, &sock_client))
        return EXIT_FAILURE;

    // Close listening socket
    if (NetworkCloseSocket(sock_listening))
        return EXIT_FAILURE;

    // While loop: accept and echo message back to client
    while (TRUE) 
    {
        if (NetworkServerReceive(sock_client))
            break;
    }

    // Close socket
    if (NetworkCloseSocket(sock_client))
        return EXIT_FAILURE;

    // End winsock
    if (NetworkWSACleanup())
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
};