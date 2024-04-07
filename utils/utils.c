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

STATUS CreateUDPSocket(SOCKET* sockfd)
{
    *sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (*sockfd == INVALID_SOCKET)
    {
        warn("Socket creation failed!");
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
