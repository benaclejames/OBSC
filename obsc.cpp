#include <obs-module.h>
#include <obs-frontend-api.h>
#include <stdio.h>
#include <thread>
#include "stored.h"
#include "socket_helper.h"
#include "osc_message.h"


OBS_DECLARE_MODULE()

socket_helper* sockets;
stored stored;
std::thread* osc_thread;

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

int create_osc_int_message(char* message, const char* address, int value) {
    int len = strlen(address)+1;
    int paddedLen = len + 4 - len % 4;
    memcpy(message, address, paddedLen);

    message[paddedLen++] = ',';
    message[paddedLen++] = 'i'; 
    message[paddedLen++] = '\0';
    message[paddedLen++] = '\0';

    // Convert value from big endian to little endian and copy to message
    char* valuePtr = (char*)&value;
    message[paddedLen++] = valuePtr[3];
    message[paddedLen++] = valuePtr[2];
    message[paddedLen++] = valuePtr[1];
    message[paddedLen++] = valuePtr[0];

    return paddedLen;
}

void update_osc()
{
    osc_message* currentMessage = new osc_bool_message((char*)"/recording", stored.get_recording_active());
    sockets->send(currentMessage->message, currentMessage->writerIndex);

    currentMessage = new osc_bool_message((char*)"/streaming", stored.get_streaming_active());
    sockets->send(currentMessage->message, currentMessage->writerIndex);

    currentMessage = new osc_int_message((char*)"/replaybuffer", stored.get_replay_buffer_save_count());
    sockets->send(currentMessage->message, currentMessage->writerIndex);
}

void frontend_cb(enum obs_frontend_event event, void *priv_data)
{
    switch (event)
    {
        case OBS_FRONTEND_EVENT_RECORDING_STARTED:
        case OBS_FRONTEND_EVENT_RECORDING_STOPPED:
        case OBS_FRONTEND_EVENT_STREAMING_STARTED:
        case OBS_FRONTEND_EVENT_STREAMING_STOPPED:
            update_osc();
            break;

        case OBS_FRONTEND_EVENT_REPLAY_BUFFER_SAVED:
            stored.increment_save_count();
            update_osc();
            break;

        case OBS_FRONTEND_EVENT_EXIT:
            osc_thread->detach();
            delete osc_thread;
            break;
    }
}

void periodic_update_loop()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        update_osc();
    }
}

bool obs_module_load()
{
    sockets = new socket_helper("127.0.0.1", 9001, 9000);
    
    obs_frontend_add_event_callback(frontend_cb, nullptr);

    // Start the osc update loop on a separate thread
    osc_thread = new std::thread(periodic_update_loop);
    return true;
}