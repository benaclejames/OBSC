#pragma once

class stored
{
    int replay_buffer_save_count = 0;
    
public:
    void increment_save_count() { replay_buffer_save_count++; }
    
    int get_replay_buffer_save_count() const { return replay_buffer_save_count; }
    bool get_recording_active() const { return obs_frontend_recording_active(); }
    bool get_streaming_active() const { return obs_frontend_streaming_active(); }
};
