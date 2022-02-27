#include <obs-module.h>
#include <obs-frontend-api.h>
#include <WinSock2.h>

#pragma comment(lib,"WS2_32")

OBS_DECLARE_MODULE()

WSADATA data;
SOCKADDR_IN addr;
SOCKET sock;

// /recording  ,F  
unsigned const char recordingAddr[16] = {0x2f, 0x72, 0x65, 0x63, 0x6f, 0x72, 0x64, 0x69, 0x6e, 0x67, 0x00, 0x00, 0x2c, 0x46, 0x00, 0x00};

// /streaming  ,F  
unsigned const char streamingAddr[16] = {0x2f, 0x73, 0x74, 0x72, 0x65, 0x61, 0x6d, 0x69, 0x6e, 0x67, 0x00, 0x00, 0x2c, 0x46, 0x00, 0x00};

void frontend_cb(enum obs_frontend_event event, void *priv_data)
{
    if (event == OBS_FRONTEND_EVENT_RECORDING_STARTED)
    {
        unsigned char sendData[16];
        memcpy(sendData, recordingAddr, sizeof(sendData));
        sendData[13] = 0x54;
        sendto(sock, sendData, sizeof(sendData), 0, (struct sockaddr*) &addr, sizeof(addr));
    }
    else if (event == OBS_FRONTEND_EVENT_RECORDING_STOPPED)
        sendto(sock, recordingAddr, sizeof(recordingAddr), 0, (struct sockaddr*) &addr, sizeof(addr));

    if (event == OBS_FRONTEND_EVENT_STREAMING_STARTED)
    {
        unsigned char sendData[16];
        memcpy(sendData, streamingAddr, sizeof(sendData));
        sendData[13] = 0x54;
        sendto(sock, sendData, sizeof(sendData), 0, (struct sockaddr*) &addr, sizeof(addr));
    }
    else if (event == OBS_FRONTEND_EVENT_STREAMING_STOPPED)
        sendto(sock, streamingAddr, sizeof(streamingAddr), 0, (struct sockaddr*) &addr, sizeof(addr));
}

bool obs_module_load(void)
{
    WSAStartup(MAKEWORD(2,2), &data);
    
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(9000);

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    obs_frontend_add_event_callback(frontend_cb, NULL);
    return true;
}