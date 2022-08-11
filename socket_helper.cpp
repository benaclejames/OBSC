#include "socket_helper.h"
#include "osc_message.h"

#include <util/base.h>

void socket_helper::recv_loop()
{
    int bytes_received;
    char serverBuf[1025];
    int serverBufLen = 1024;
    
    // Keep a seperate address struct to store sender information. 
    struct sockaddr_in SenderAddr;
    int SenderAddrSize = sizeof (SenderAddr);

    blog(LOG_INFO,"Receiving datagrams");
    while (true)
    {
        bytes_received = recvfrom(inSock, serverBuf, serverBufLen, 0 /* no flags*/, (SOCKADDR *) & SenderAddr, &SenderAddrSize);
        osc_message msg = osc_message(serverBuf, bytes_received);
        onMsgRecv(msg);
    }
}

socket_helper::socket_helper(std::string ip, int inPort, int outPort, void(*onMsgRecv)(osc_message)) : onMsgRecv(onMsgRecv)
{
    WSAStartup(MAKEWORD(2,2), &data);
    
    outAddr.sin_family = AF_INET;
    InetPton(AF_INET, ip.c_str(), &outAddr.sin_addr.s_addr);
    outAddr.sin_port = htons(outPort);

    outSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    inAddr.sin_family = AF_INET;
    inAddr.sin_port = htons(inPort);
    InetPton(AF_INET, ip.c_str(), &inAddr.sin_addr.s_addr);

    inSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if(bind(inSock, (SOCKADDR*)&inAddr, sizeof(inAddr)))
        blog(LOG_ERROR, "[OBSC] Failed to bind in socket");

    recvThread = new std::thread(&socket_helper::recv_loop, this);
}

socket_helper::~socket_helper() {
    recvThread->detach();
    delete recvThread;

    closesocket(inSock);
    closesocket(outSock);
    WSACleanup();
}
