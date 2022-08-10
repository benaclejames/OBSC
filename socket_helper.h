#pragma once
#include <string>
#include <thread>
#include <WS2tcpip.h>
#include <WinSock2.h>

#pragma comment(lib,"WS2_32")

class socket_helper
{
    WSADATA data;
    SOCKADDR_IN outAddr, inAddr;
    SOCKET inSock, outSock;
    std::thread* recvThread;

    void recv_loop();
public:
    socket_helper(std::string ip, int inPort, int outPort);

    void send(char* data, int size)
    {
        sendto(outSock, data, size, 0, (SOCKADDR*)&outAddr, sizeof(outAddr));
    }
};
