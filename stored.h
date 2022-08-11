#pragma once

struct stored
{
    int replay_buffer_save_count = 0;
    
public:
    inline void increment_save_count() { replay_buffer_save_count++; }
    
    inline int get_replay_buffer_save_count() const { return replay_buffer_save_count; }
    inline bool get_recording_active() const { return obs_frontend_recording_active(); }
    inline bool get_streaming_active() const { return obs_frontend_streaming_active(); }
    inline bool get_replay_buffer_active() const { return obs_frontend_replay_buffer_active(); }
    inline int get_scene_index() {
        obs_frontend_source_list_free(&sceneList);
        obs_frontend_get_scenes(&sceneList);
        for (size_t i = 0; i < sceneList.sources.num; i++) {
            obs_source_t* source = sceneList.sources.array[i];
            if (source == obs_frontend_get_current_scene())
                return i;
        }
        return -1;
    }

    obs_frontend_source_list sceneList = {};
};
