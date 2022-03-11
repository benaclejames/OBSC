#include <obs-module.h>
#include <obs-frontend-api.h>
#include <stdio.h>
#include <WS2tcpip.h>
#include <WinSock2.h>

#pragma comment(lib,"WS2_32")

OBS_DECLARE_MODULE()

WSADATA data;
SOCKADDR_IN addr;
SOCKET sock;

int create_osc_bool_message(char* message, const char* address, bool value) {
    int len = strlen(address)+1;
    int paddedLen = len + 4 - len % 4;
    memcpy(message, address, paddedLen);

    message[paddedLen++] = ',';
    message[paddedLen++] = value ? 'T' : 'F'; 
    message[paddedLen++] = '\0';
    message[paddedLen++] = '\0';

    return paddedLen;
}

void frontend_cb(enum obs_frontend_event event, void *priv_data)
{
    char message[100];
    int msgLen;
    
    switch (event)
    {
        case OBS_FRONTEND_EVENT_RECORDING_STARTED:
        case OBS_FRONTEND_EVENT_RECORDING_STOPPED:
            msgLen = create_osc_bool_message(message, "/recording", event == OBS_FRONTEND_EVENT_RECORDING_STARTED);
            break;
        
        case OBS_FRONTEND_EVENT_STREAMING_STARTED:
        case OBS_FRONTEND_EVENT_STREAMING_STOPPED:
            msgLen = create_osc_bool_message(message, "/streaming", event == OBS_FRONTEND_EVENT_STREAMING_STARTED);
            break;

        default:
            return;
    }

    sendto(sock, message, msgLen, 0, (struct sockaddr*) &addr, sizeof(addr));
}

bool obs_module_load(void)
{
    WSAStartup(MAKEWORD(2,2), &data);
    
    addr.sin_family = AF_INET;
    InetPton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);
    addr.sin_port = htons(9000);

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    obs_frontend_add_event_callback(frontend_cb, NULL);
    return true;
}