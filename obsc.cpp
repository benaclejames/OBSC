#include <obs-module.h>
#include <obs-frontend-api.h>
#include <functional>
#include <queue>
#include <mutex>

#include "stored.h"
#include "socket_helper.h"
#include "osc_message.h"


OBS_DECLARE_MODULE()

typedef std::function<void(void)> task_t;
std::queue<task_t> tasks;
std::mutex tasks_mutex;

socket_helper* sockets;
stored stored_data;

void update_osc()
{
    // Are we recording?
    osc_message* currentMessage = new osc_bool_message((char*)"/avatar/parameters/OBSRecordToggle", stored_data.get_recording_active());
    sockets->send(currentMessage->message, currentMessage->writerIndex);

    // Are we streaming?
    currentMessage = new osc_bool_message((char*)"/avatar/parameters/OBSStreamToggle", stored_data.get_streaming_active());
    sockets->send(currentMessage->message, currentMessage->writerIndex);

    // Is replay buffer currently recording?
    currentMessage = new osc_bool_message((char*)"/avatar/parameters/OBSReplayBufferToggle", stored_data.get_replay_buffer_active());
    sockets->send(currentMessage->message, currentMessage->writerIndex);

    // How many times has the replay buffer been saved?
    currentMessage = new osc_int_message((char*)"/avatar/parameters/OBSReplayBufferSaveCount", stored_data.get_replay_buffer_save_count());
    sockets->send(currentMessage->message, currentMessage->writerIndex);

    // What index is the scene currently at?
    float sceneIndex = stored_data.get_scene_index() / 100.0f;
    currentMessage = new osc_float_message((char*)"/avatar/parameters/OBSSceneSwitchSelector", sceneIndex);
    sockets->send(currentMessage->message, currentMessage->writerIndex);
}

void handle_scene_switch(int newSceneIndex) {
    if (stored_data.get_scene_index() == newSceneIndex)
        return;

    if (stored_data.sceneList.sources.array != nullptr && stored_data.sceneList.sources.num > newSceneIndex) {
        obs_source_t *source = stored_data.sceneList.sources.array[newSceneIndex];
        if (source != nullptr)
            obs_frontend_set_current_scene(source);
    }
    else {  // We're out of range, relay this info back to the client
        float sceneIndex = stored_data.get_scene_index() / 100.0f;
        osc_message* currentMessage = new osc_float_message((char*)"/avatar/parameters/OBSSceneSwitchSelector", sceneIndex);
        sockets->send(currentMessage->message, currentMessage->writerIndex);
    }
}

void frontend_cb(enum obs_frontend_event event, void *priv_data)
{
    blog(LOG_INFO, "frontend_cb: %d", event);
    switch (event)
    {
        case OBS_FRONTEND_EVENT_RECORDING_STARTED:
        case OBS_FRONTEND_EVENT_RECORDING_STOPPED:
        case OBS_FRONTEND_EVENT_STREAMING_STARTED:
        case OBS_FRONTEND_EVENT_STREAMING_STOPPED:
        case OBS_FRONTEND_EVENT_REPLAY_BUFFER_STARTED:
        case OBS_FRONTEND_EVENT_REPLAY_BUFFER_STOPPED:
        case OBS_FRONTEND_EVENT_SCENE_CHANGED:
            update_osc();
            break;

        case OBS_FRONTEND_EVENT_REPLAY_BUFFER_SAVED:
            stored_data.increment_save_count();
            update_osc();
            break;

        case OBS_FRONTEND_EVENT_EXIT:
            delete sockets;
            break;
    }
}

void on_update_recv(osc_message message) {
    blog(LOG_INFO, "Received update message for address %s", message.address);

    if (strcmp(message.address, "/avatar/change") == 0)
        update_osc();

    if (strcmp(message.address, "/avatar/parameters/OBSRecordToggle") == 0) {
        if (message.type == 'T' && !stored_data.get_recording_active()) {
            obs_frontend_recording_start();
            blog(LOG_INFO, "Recording started");
        } else if (message.type == 'F' && stored_data.get_recording_active()) {
            obs_frontend_recording_stop();
            blog(LOG_INFO, "Recording stopped");
        }
    }

    if (strcmp(message.address, "/avatar/parameters/OBSStreamToggle") == 0) {
        if (message.type == 'T' && !stored_data.get_streaming_active())
            obs_frontend_streaming_start();
        else if (message.type == 'F' && stored_data.get_streaming_active())
            obs_frontend_streaming_stop();
    }

    if (strcmp(message.address, "/avatar/parameters/OBSReplayBufferToggle") == 0) {
        if (message.type == 'T' && !stored_data.get_replay_buffer_active())
            obs_frontend_replay_buffer_start();
        else if (message.type == 'F' && stored_data.get_replay_buffer_active())
            obs_frontend_replay_buffer_stop();
    }

    if (strcmp(message.address, "/avatar/parameters/OBSReplayBufferCapture") == 0 && message.type == 'T')
            obs_frontend_replay_buffer_save();

    if (strcmp(message.address, "/avatar/parameters/OBSScreenshotCapture") == 0 && message.type == 'T')
        obs_frontend_take_screenshot();

    if (strcmp(message.address, "/avatar/parameters/OBSSceneSwitchSelector") == 0 && message.type == 'f') {
        // We'll have to read the value from the message
        osc_float_message* intMessage = (osc_float_message*)&message;
        int newSceneIndex = intMessage->parse()*100;
        blog(LOG_INFO, "Switching to scene %d", newSceneIndex);
        std::unique_lock<std::mutex> lock(tasks_mutex);
        task_t task = [newSceneIndex]() {
            handle_scene_switch(newSceneIndex);
        };
        tasks.push(task);
    }
}

void update_loop(void* data, float seconds) {
    while(!tasks.empty()) {
        auto task = tasks.front();
        tasks.pop();
        task();
    }
}

bool obs_module_load()
{
    obs_frontend_source_list sceneList = {};
    obs_frontend_get_scenes(&sceneList);
    // Log how many scenes we have
    blog(LOG_INFO, "There are %d scenes", sceneList.sources.num);

    sockets = new socket_helper("127.0.0.1", 9001, 9000, on_update_recv);

    obs_add_tick_callback(update_loop, &stored_data);
    obs_frontend_add_event_callback(frontend_cb, nullptr);

    return true;
}