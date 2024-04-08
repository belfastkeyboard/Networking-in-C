#include "utils/utils.h"

#define IP_ADDRESS "127.0.0.1"
#define PORT 54000

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

    char buffer[BUFFER_LEN];
    char* user_input;
    size_t len, size;
    int bytes_sent, bytes_rcvd, error;
    time_t current_time;
    struct tm *local_time;

    // Do-while loop to send-receive data
    do {
        user_input = NULL;
        size = 0;
        len = 0;
        
        printf("> ");
        len = getline(&user_input, &size, stdin);

        if (len - 1 > 0)
        {
            bytes_sent = send(sockfd, user_input, len, SEND_FLAG);

            if (bytes_sent == SOCKET_ERROR)
            {
                warn("Error sending.", 0);
                error = WSAGetLastError();
                PrintWSAErrorMessage(error);
                break;
            }
            else
            {
                memset(buffer, 0, BUFFER_LEN);
                bytes_rcvd = recv(sockfd, buffer, BUFFER_LEN, RECV_FLAG);

                if (bytes_rcvd == SOCKET_ERROR)
                {
                    warn("Error receiving.", 0);
                    error = WSAGetLastError();
                    PrintWSAErrorMessage(error);
                    break;
                }
                else if (bytes_rcvd == 0)
                {
                    info("Connection closed.", 0);
                    break;
                }
                else
                {
                    printf("SERVER: %s", buffer);
                }
            }
        }

        free(user_input);
    } while (len - 1 > 1);

    // Shut down
    if (NetworkCloseSocket(sockfd))
        return EXIT_FAILURE;

    if (NetworkWSACleanup())
        return EXIT_FAILURE;

    return 0;
}