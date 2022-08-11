#include <obs-module.h>
#include <obs-frontend-api.h>
#include <thread>
#include "stored.h"
#include "socket_helper.h"
#include "osc_message.h"


OBS_DECLARE_MODULE()

socket_helper* sockets;
stored stored;

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
            delete sockets;
            break;
    }
}

void on_update_recv(osc_message message) {
    if (strcmp(message.address, "/avatar/change") == 0)
        update_osc();
}

bool obs_module_load()
{
    sockets = new socket_helper("127.0.0.1", 9001, 9000, on_update_recv);
    
    obs_frontend_add_event_callback(frontend_cb, nullptr);

    return true;
}