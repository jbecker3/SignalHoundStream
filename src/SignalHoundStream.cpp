//
// Created by Jonathan Becker on 2/6/2024.
//

#include "SignalHoundStream.h"

#include <cassert>


SignalHoundStream::SignalHoundStream(){
    device = -1;
    setupDeviceUSB();
}

void SignalHoundStream::setupDeviceUSB() {
    status = smOpenDevice(&device);
}

void SignalHoundStream::setupDeviceNetwork() {
    status = smOpenDevice(&device);
}

void SignalHoundStream::getVRTPackets(int dataPacketCount,IQ_Parameters parameters) {
    assert(status == smNoError);

    // Set IQ parameters
    smSetIQCenterFreq(device, parameters.centerFreq);
    smSetIQSampleRate(device, parameters.sampleRate);
    smSetIQBandwidth(device, smTrue, parameters.iqBandwidth);
    smSetRefLevel(device, parameters.refLevel);

    // Set VRt parameters
    smSetVrtStreamID(device, parameters.streamID);
    smSetVrtPacketSize(device, parameters.packetSize);

    // Configure
    status = smConfigure(device, smModeIQStreaming); // VRT mode
    assert(status == smNoError);

    // Allocate memory
    uint32_t contextWordCount;
    status = smGetVrtContextPktSize(device, &contextWordCount);
    assert(status == smNoError);

    uint16_t samplesPerPkt;
    uint32_t dataWordCount;
    status = smGetVrtPacketSize(device, &samplesPerPkt, &dataWordCount);
    assert(status == smNoError);

    uint32_t wordCount = contextWordCount + dataWordCount * dataPacketCount;
    uint32_t *words = new uint32_t[wordCount];
    uint32_t *curr = words;

    // Get context packet
    uint32_t actualContextWordCount;
    status = smGetVrtContextPkt(device, curr, &actualContextWordCount);
    assert(status == smNoError);
    assert(actualContextWordCount == contextWordCount);
    curr += contextWordCount;

    // Get data packets
    uint32_t actualDataWordCount;
    status = smGetVrtPackets(device, curr, &actualDataWordCount, dataPacketCount, smFalse);
    assert(status == smNoError);
    assert(actualDataWordCount == dataWordCount * dataPacketCount);

    smCloseDevice(device);

    if(words) delete[] words;
}
