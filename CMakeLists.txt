# OBSC/CMakeLists.txt

project(OBSC)

set(OBSC_HEADERS
      stored.h
)

set(OBSC_SOURCES
      obsc.cpp
)

link_directories($ENV{OBS_DIR}/build/libobs/Debug $ENV{OBS_DIR}/build/UI/obs-frontend-api/Debug)

add_library(OBSC MODULE
      ${OBSC_SOURCES}
      ${OBSC_HEADERS})

target_include_directories(OBSC PRIVATE $ENV{OBS_DIR}/libobs $ENV{OBS_DIR}/UI/obs-frontend-api)

target_link_libraries(OBSC
      obs obs-frontend-api)