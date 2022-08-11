# 🎥 OBSC
OBSC is a plugin for OBS-Studio that allows for sending and receiving data over Open Sound Protocol regarding current states as well as being able to control said states.

## ✨Features
- Boolean status for Recording, Streaming, and Replay Buffer
- Integer status for Current Scene Index and Replay Buffer trigger count in the current session
- Boolean triggers for Screenshot and Replay Buffer capture

## 💻 Setup
Just drag `OBSC.dll` into your obs-studio plugins folder. This can usually be found at `C:\Program Files\obs-studio\obs-plugins\64bit`
> **_NOTE:_**  Streamlabs OBS doesn't support custom plugins. OBSC only works with obs-studio.

## 📜 OSC Parameters:
| Address  | Type | I/O | Description
| ------------- | ------------- | ------------ |-----|
| /avatar/parameters/OBSRecordToggle  | Boolean  | In+Out | Are we currently recording? |
| /avatar/parameters/OBSStreamToggle  | Boolean  | In+Out | Are we currently streaming? |
| /avatar/parameters/OBSReplayBufferToggle  | Boolean  | In+Out | Is the replay buffer currently active? |
| /avatar/parameters/OBSReplayBufferSaveCount  | Integer  | Out | How many times have we triggered the replay buffer this session? |
| /avatar/parameters/OBSSceneSwitchSelector  | Integer  | In+Out | Get/Set the current scene index. (Starts at 0) |
| /avatar/parameters/OBSReplayBufferCapture  | Boolean  | In | Save the current replay buffer. |
| /avatar/parameters/OBSScreenshotCapture  | Boolean  | In | Take a screenshot |


## 🥽 VRChat
If you intend to use VRChat to interface with OBSC, an avatar setup guide can be found on [the wiki](https://github.com/benaclejames/OBSC/wiki)
