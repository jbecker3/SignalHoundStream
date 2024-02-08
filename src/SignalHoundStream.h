//
// Created by Jonathan Becker on 2/6/2024.
//

#ifndef SIGNALHOUNDSTREAM_SIGNALHOUNDSTREAM_H
#define SIGNALHOUNDSTREAM_SIGNALHOUNDSTREAM_H

#include "sm_api.h"
#include "sm_api_vrt.h"

#include <string>
#include <vector>

#pragma comment(lib, "sm_api")

struct IQ_Parameters{
    double center_freq;
    int sample_rate;
    double iq_bandwidth;
    double ref_level;
    int stream_id;
    int packet_size;
};

class SignalHoundStream {
private:
    int device;
    SmStatus status;
    std::vector<uint32_t> words;

public:
    SignalHoundStream();
    ~SignalHoundStream();
    void setupDeviceUSB();
    void setupDeviceNetwork();
    void getVRTPackets(int packet_count, IQ_Parameters parameters);
    void writeStream(const std::string& filename);
};


#endif //SIGNALHOUNDSTREAM_SIGNALHOUNDSTREAM_H
