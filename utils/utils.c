#include "utils.h"

void PrintWSAErrorMessage(int error)
{
    DWORD result;
    LPSTR error_message;
    result = FormatMessageA(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                error,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPSTR)&error_message,
                0,
                NULL);
    if (result != 0)
    {
        warn("Error code: %d.", error);
        warn("Error message: %s", error_message);
    }
}

STATUS NetworkWSAInitialisation(WSADATA* wsa_data)
{
    WORD ver;
    int result;
    
    ver = MAKEWORD(2, 2);

    result = WSAStartup(ver, wsa_data);

    if (result == WSA_SUCCESS)
    {
        return result;
    }
    else
    {
        warn("Can't initialise WSA.", 0);
        PrintWSAErrorMessage(result);
        return result;
    }
}
STATUS NetworkWSACleanup(void)
{
    int result, error;

    result = WSACleanup();

    if (result == WSA_SUCCESS)
    {
        return result;
    }
    else
    {
        warn("Failed WSA cleanup.", 0);
        error = WSAGetLastError();
        PrintWSAErrorMessage(error);
        return result;
    }
}

STATUS NetworkCreateSocket(SOCKET* sockfd, int af, int type, int protocol)
{
    int error;

    *sockfd = socket(af, type, protocol);

    if (*sockfd == INVALID_SOCKET)
    {
        warn("Socket creation failed!", 0);
        error = WSAGetLastError();
        PrintWSAErrorMessage(error);
        return EXIT_FAILURE;
    }
    else
    {
        return EXIT_SUCCESS;
    }
}
STATUS NetworkCloseSocket(SOCKET sockfd)
{
    int result, error;

    result = closesocket(sockfd);

    if (result == WSA_SUCCESS)
    {
        return result;
    }
    else
    {
        warn("Failed to close socket %d.", sockfd);
        error = WSAGetLastError();
        PrintWSAErrorMessage(error);
        return result;
    }
}

STATUS NetworkBindSocket(SOCKET sockfd, struct sockaddr_in* addr)
{
    int result, error;

    result = bind(sockfd, (const struct sockaddr*)addr, sizeof(*addr));

    if (result == WSA_SUCCESS)
    {
        return result;
    }
    else
    {
        error = WSAGetLastError();
        PrintWSAErrorMessage(error);
        return result;
    }
}

STATUS NetworkListenSocket(SOCKET sockfd, int backlog)
{
    int result, error;

    result = listen(sockfd, backlog);

    if (result == WSA_SUCCESS)
    {
        return result;
    }
    else
    {
        error = WSAGetLastError();
        PrintWSAErrorMessage(error);
        return result;
    }
}
STATUS NetworkConnect(SOCKET sockfd, struct sockaddr_in* addr)
{
    int result, error;

    result = connect(sockfd, (const struct sockaddr*)addr, sizeof(*addr));

    if (result == SOCKET_ERROR)
    {
        error = WSAGetLastError();
        PrintWSAErrorMessage(error);
        return result;
    }
    else
    {
        return result;
    }
}
STATUS NetworkWaitForConnection(struct sockaddr_in* addr, SOCKET serv, SOCKET* recv)
{
    int client_len, error;
    char host[NI_MAXHOST]; // Client remote name
    char service [NI_MAXSERV]; // Client service (i.e. port)
    DWORD len;

    memset(addr, 0, sizeof(*addr));

    client_len = sizeof(*addr);
    *recv = accept(serv, (struct sockaddr*)addr, &client_len);

    if (*recv == INVALID_SOCKET)
    {
        error = WSAGetLastError();
        PrintWSAErrorMessage(error);
        return EXIT_FAILURE;
    }

    memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXSERV);

    if (getnameinfo((const struct sockaddr*)&addr, sizeof(addr), host, NI_MAXHOST, service, NI_MAXSERV, 0))
    {
        if (!strlen(host))
            strcpy(host, "Unknown");
            
        len = NI_MAXHOST;

        WSAAddressToStringA((struct sockaddr*)&addr, sizeof(addr), NULL, host, &len);
        info("%s connected on port %hu", host, ntohs(addr->sin_port));
    }
    else
    {
        info("%s connected on port %s", host, service);
    }

    return EXIT_SUCCESS;
}
STATUS NetworkServerReceive(SOCKET sockfd)
{
    char buffer[BUFFER_LEN];
    int bytes_rcvd, bytes_sent, error;
    time_t current_time;
    struct tm *local_time;

    while (TRUE)
    {
        memset(buffer, 0, BUFFER_LEN);

        bytes_rcvd = recv(sockfd, buffer, BUFFER_LEN, RECV_FLAG);
        
        time(&current_time);
        local_time = localtime(&current_time);

        printf("[%02d:%02d:%02d] Message received: %s", local_time->tm_hour, local_time->tm_min, local_time->tm_sec, buffer);

        if (bytes_rcvd == SOCKET_ERROR)
        {
            warn("Error in recv(). Exiting.", 0);
            return EXIT_FAILURE;
        }
        else if (bytes_rcvd == 0)
        {
            info("Client disconnected!", 0);
            return EXIT_SUCCESS;
        }
        else
        {
            // send message back to the client - think echo command
            bytes_sent = send(sockfd, buffer, bytes_rcvd + 1, SEND_FLAG);

            time(&current_time);
            local_time = localtime(&current_time);

            if (bytes_sent == SOCKET_ERROR)
            {
                error = WSAGetLastError();
                PrintWSAErrorMessage(error);
                return EXIT_FAILURE;
            }
            else
            {
                printf("[%02d:%02d:%02d] Message sent: %s", local_time->tm_hour, local_time->tm_min, local_time->tm_sec, buffer);
            }
        }
    }

    return EXIT_SUCCESS;
}

STATUS NetworkClientSend(SOCKET sockfd)
{
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
                error = WSAGetLastError();

                if (error == 10054)
                {
                    warn("Connection with server lost.", 0);
                }
                else
                {
                    warn("Error sending.", 0);
                    PrintWSAErrorMessage(error);
                }

                return EXIT_FAILURE;
            }
            else
            {
                memset(buffer, 0, BUFFER_LEN);
                bytes_rcvd = recv(sockfd, buffer, BUFFER_LEN, RECV_FLAG);

                time(&current_time);
                local_time = localtime(&current_time);

                if (bytes_rcvd == SOCKET_ERROR)
                {
                    warn("Error receiving.", 0);
                    error = WSAGetLastError();
                    PrintWSAErrorMessage(error);
                    return EXIT_FAILURE;
                }
                else if (bytes_rcvd == 0)
                {
                    info("Connection closed.", 0);
                    return EXIT_SUCCESS;
                }
                else
                {
                    printf("[%02d:%02d:%02d] SERVER: %s", local_time->tm_hour, local_time->tm_min, local_time->tm_sec, buffer);
                }
            }
        }

        free(user_input);
    } while (len - 1 > 1);

    return EXIT_SUCCESS;
}

DWORD WINAPI NetworkThreadClientSend(LPVOID lpParam)
{
    SOCKET sockfd;
    char* user_input;
    size_t len, size;
    int bytes_sent, error;

    sockfd = *(SOCKET*)lpParam;

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
                return EXIT_FAILURE;
            }
        }

        free(user_input);
    } while (len - 1 > 1);

    return EXIT_SUCCESS;
}
DWORD WINAPI NetworkThreadClientReceive(LPVOID lpParam)
{
    SOCKET sockfd;
    char buffer[BUFFER_LEN];
    int bytes_rcvd;
    time_t current_time;
    struct tm *local_time;

    sockfd = *(SOCKET*)lpParam;

    while (TRUE)
    {
        memset(buffer, 0, BUFFER_LEN);

        bytes_rcvd = recv(sockfd, buffer, BUFFER_LEN, RECV_FLAG);
        
        time(&current_time);
        local_time = localtime(&current_time);

        if (bytes_rcvd == SOCKET_ERROR)
        {
            warn("Error in recv(). Exiting.", 0);
            return EXIT_FAILURE;
        }
        else if (bytes_rcvd == 0)
        {
            info("Server disconnected?", 0);
            return EXIT_SUCCESS;
        }
        else
        {
            printf("[%02d:%02d:%02d] Message received: %s", local_time->tm_hour, local_time->tm_min, local_time->tm_sec, buffer);
        }
    }

    return EXIT_SUCCESS;
}

STATUS NetworkClientSendReceive(SOCKET sockfd)
{
    HANDLE HANDLES[2];
    HANDLE sendHANDLE, receiveHANDLE;
    LPVOID sendARGS, receiveARGS;
    DWORD sendID, receiveID;

    sendARGS = (LPVOID)&sockfd;
    receiveARGS = (LPVOID)&sockfd;

    sendHANDLE = CreateThread(THREAD_NO_SECURITY_ATTRS, THREAD_MEM_SIZE, NetworkThreadClientSend, sendARGS, THREAD_START_NO_DELAY, &sendID);

    if (sendHANDLE == NULL)
    {
        warn("Thread '%lu' creation failed!", sendID);
        return EXIT_FAILURE;
    }
    // else
    // {
    //     good("Thread '%lu' created!", sendID);
    // }

    receiveHANDLE = CreateThread(THREAD_NO_SECURITY_ATTRS, THREAD_MEM_SIZE, NetworkThreadClientReceive, receiveARGS, THREAD_START_NO_DELAY, &receiveID);

    if (receiveHANDLE == NULL)
    {
        warn("Thread '%lu' creation failed!", receiveID);
        return EXIT_FAILURE;
    }
    // else
    // {
    //     good("Thread '%lu' created!", receiveID);
    // }

    HANDLES[0] = sendHANDLE;
    HANDLES[1] = receiveHANDLE;
    WaitForMultipleObjects(2, HANDLES, THREAD_WAIT_FOR_ALL, INFINITE);

    return EXIT_SUCCESS;
}

void NetworkConstructSockaddr_in(struct sockaddr_in *addr, short fam, u_short port, u_long S_addr)
{
    memset(addr, 0, sizeof(*addr));
    addr->sin_family = fam;
    addr->sin_port = htons(port);
    addr->sin_addr.S_un.S_addr = S_addr;
}

STATUS CreateUDPSocket(SOCKET* sockfd)
{
    *sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (*sockfd == INVALID_SOCKET)
    {
        warn("Socket creation failed!", 0);
        PrintWSAErrorMessage(WSAGetLastError());
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

void GetIPAddr(char* ipaddr)
{
        char hostname[256];
        struct hostent* host;
        struct in_addr addr;

        gethostname(hostname, sizeof(hostname));

        host = gethostbyname(hostname);

        memcpy(&addr, host->h_addr_list[0], host->h_length);

        strcpy(ipaddr, inet_ntoa(addr));
}
