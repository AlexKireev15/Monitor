#pragma once

#define WIN32_LEAN_AND_MEAN
#define no_init_all
#define NOMINMAX

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#ifdef SOCKET_DEBUG_INFO
    #define SOCKET_DEBUG(args) {args;}
#else
    #define SOCKET_DEBUG(args) /* no call */
#endif
