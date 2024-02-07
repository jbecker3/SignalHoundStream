//
// Created by Jonathan Becker on 2/6/2024.
//

#ifndef SIGNALHOUNDSTREAM_SIGNALHOUNDSTREAM_H
#define SIGNALHOUNDSTREAM_SIGNALHOUNDSTREAM_H

#include "sm_api.h"
#include "sm_api_vrt.h"

#pragma comment(lib, "sm_api")

struct IQ_Parameters{
    double centerFreq;
    int sampleRate;
    double iqBandwidth;
    int refLevel;
    int streamID;
    int packetSize;
};

class SignalHoundStream {
private:
    int device;
    SmStatus status;

public:
    SignalHoundStream();
    ~SignalHoundStream()= default;
    void setupDeviceUSB();
    void setupDeviceNetwork();
    void getVRTPackets(int dataPacketCount, IQ_Parameters parameters);
};


#endif //SIGNALHOUNDSTREAM_SIGNALHOUNDSTREAM_H
