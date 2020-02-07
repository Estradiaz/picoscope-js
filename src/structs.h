#ifndef PICO_STRUCTS_H
#define PICO_STRUCTS_H

#include <napi.h>

#define OVERVIEW_BUFFER_SIZE 60000
#define CHANNEL_A 0
#define CHANNEL_B 1

#define TEST false

struct Channel {
    
    int16_t dc;
    int16_t direction;
    int16_t range;
    int16_t enabled;
};

struct OverviewBufferLists {
    uint32_t nValues;
    std::vector<int16_t> minA;
    std::vector<int16_t> maxA;
    std::vector<int16_t> minB;
    std::vector<int16_t> maxB;
};

struct Channels {
    Channel* A;
    Channel* B;
};

struct DeviceState {
    int16_t deviceHandle;
    int16_t *read;
    Channel *A;
    Channel *B;
};


#endif