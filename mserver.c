#include "utils/utils.h"

#define DEPRECATED 0

int main(void)
{
    WSADATA data;
    SOCKET sock_listening, sock_client;
    struct sockaddr_in addr;

    // Begin winsock
    if (NetworkWSAInitialisation(&data))
        return EXIT_FAILURE;

    // Create socket
    if (NetworkCreateSocket(&sock_listening, AF_INET, SOCK_STREAM, IPPROTO_IP))
        return EXIT_FAILURE;

    // Bind socket to IP addr and port
    NetworkConstructSockaddr_in(&addr, AF_INET, PORT, INADDR_ANY);
    if (NetworkBindSocket(sock_listening, &addr))
        return EXIT_FAILURE;

    // Tell winsock the socket is listening
    if (NetworkListenSocket(sock_listening, SOMAXCONN))
        return EXIT_FAILURE;

    fd_set master;

    FD_ZERO(&master);

    FD_SET(sock_listening, &master);

    while (TRUE)
    {
        int socket_count;
        fd_set worker;
        SOCKET sockfd;

        worker = master;

        socket_count = select(DEPRECATED, &worker, NULL, NULL, NULL); // timeout ?

        for (int i = 0; i < socket_count; i++)
        {
            sockfd = worker.fd_array[i];

            if (sockfd == sock_listening)
            {
                SOCKET client;
                char welcome[21];
                
                // accept new connection
                client = accept(sock_listening, NULL, NULL);

                // add connection to list of connected clients
                FD_SET(client, &master);

                // send welcome message to client
                strcpy(welcome, "Welcome to the chat!");
                int check = send(client, welcome, 21, SEND_FLAG);

                // TODO: broadcast message to other clients welcoming new user
                for (int j = 0; j < master.fd_count; j++)
                {
                    SOCKET user; 
                    char welcome[23];

                    user = master.fd_array[j];
                    strcpy(welcome, "A new user has joined!");

                    if (user == client || user == sock_listening)
                        continue;

                    send(user, welcome, 23, SEND_FLAG);
                }
            }
            else
            {
                // accept new message
                char buffer[BUFFER_LEN];
                int bytes_rcvd;

                memset(buffer, 0, BUFFER_LEN);

                bytes_rcvd = recv(sockfd, buffer, BUFFER_LEN, RECV_FLAG);

                if (bytes_rcvd <= 0)
                {
                    closesocket(sockfd); // error handling later
                    FD_CLR(sockfd, &master);
                }
                else
                {
                    SOCKET user; 

                    for (int i = 0; i < master.fd_count; i++)
                    {
                        user = master.fd_array[i];

                        if (user == user || user == sock_listening)
                            continue;

                        send(user, buffer, bytes_rcvd, SEND_FLAG);
                    }

                }
            }
        }
    }


    // Close socket
    if (NetworkCloseSocket(sock_client))
        return EXIT_FAILURE;

    // End winsock
    if (NetworkWSACleanup())
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
};