#ifndef NETWORKING_STATUS
#define NETWORKING_STATUS 1

#include <stdio.h>

#define good(msg, ...) printf("[+] " msg "\n", __VA_ARGS__)
#define info(msg, ...) printf("[i] " msg "\n", __VA_ARGS__)
#define warn(msg, ...) printf("[!] " msg "\n", __VA_ARGS__)

#endif