#include "socket_helper.h"

#include <util/base.h>

void socket_helper::recv_loop()
{
    int bytes_received;
    char serverBuf[1025];
    int serverBufLen = 1024;
    
    // Keep a seperate address struct to store sender information. 
    struct sockaddr_in SenderAddr;
    int SenderAddrSize = sizeof (SenderAddr);

    printf("Receiving datagrams on %s\n", "127.0.0.1");
    while (true)
    {
        bytes_received = recvfrom(inSock, serverBuf, serverBufLen, 0 /* no flags*/, (SOCKADDR *) & SenderAddr, &SenderAddrSize);
        if (bytes_received == SOCKET_ERROR) {
            printf("recvfrom failed with error %d\n", WSAGetLastError());
        }
        printf("recv bytes");
    }
}

socket_helper::socket_helper(std::string ip, int inPort, int outPort)
{
    WSAStartup(MAKEWORD(2,2), &data);
    
    outAddr.sin_family = AF_INET;
    InetPton(AF_INET, ip.c_str(), &outAddr.sin_addr.s_addr);
    outAddr.sin_port = htons(outPort);

    outSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    inAddr.sin_family = AF_INET;
    inAddr.sin_port = htons(inPort);
    InetPton(AF_INET, ip.c_str(), &inAddr.sin_addr.s_addr);

    if(bind(inSock, (SOCKADDR*)&inAddr, sizeof(inAddr)))
        blog(LOG_ERROR, "[OBSC] Failed to bind in socket");

    recvThread = new std::thread(&socket_helper::recv_loop, this);
}
