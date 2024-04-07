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
        printf("[i] Error code: %d.\n[i] Error message: %s", error, error_message);
    }
}

STATUS NetworkWSAInitialisation(void)
{
    WSADATA wsa_data;
    WORD ver;
    int result;
    
    ver = MAKEWORD(2, 2);

    result = WSAStartup(ver, &wsa_data);

    if (result == WSA_SUCCESS)
    {
        return result;
    }
    else
    {
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
        error = WSAGetLastError();
        PrintWSAErrorMessage(error);
        return result;
    }
}

STATUS NetworkCreateSocket(SOCKET* sockfd, int af, int type, int protocol)
{
    *sockfd = socket(af, type, protocol);

    if (*sockfd == INVALID_SOCKET)
    {
        warn("Socket creation failed!", 0);
        PrintWSAErrorMessage(WSAGetLastError());
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
        error = WSAGetLastError();
        PrintWSAErrorMessage(error);
        return result;
    }
}

STATUS NetworkBindSocket(SOCKET sockfd, struct sockaddr_in* addr, int addrlen)
{
    int result, error;

    result = bind(sockfd, (const struct sockaddr*)addr, addrlen);

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

        len = NI_MAXHOST;

        WSAAddressToStringA((struct sockaddr*)&addr, sizeof(addr), NULL, host, &len);
        info("%s connected on port %s", host, ntohs(addr->sin_port));
    }
    else
    {
        info("%s connected on port %s", host, service);
    }

    return EXIT_SUCCESS;
}

void NetworkConstructSockaddr_in(struct sockaddr_in* addr, short fam, u_short port, u_long S_addr)
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
