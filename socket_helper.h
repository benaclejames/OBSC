#pragma once
#include <string>
#include <thread>
#include <WS2tcpip.h>
#include <WinSock2.h>
#include "osc_message.h"

#pragma comment(lib,"WS2_32")

class socket_helper
{
    WSADATA data;
    SOCKADDR_IN outAddr, inAddr;
    SOCKET inSock, outSock;
    std::thread* recvThread;
    void (*onMsgRecv)(osc_message);

    void recv_loop();
public:
    socket_helper(std::string ip, int inPort, int outPort, void(*onMsgRecv)(osc_message));
    ~socket_helper();

    void send(char* data, int size)
    {
        sendto(outSock, data, size, 0, (SOCKADDR*)&outAddr, sizeof(outAddr));
    }
};
