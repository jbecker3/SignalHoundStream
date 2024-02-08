//
// Created by Jonathan Becker on 2/6/2024.
//

#include "SignalHoundStream.h"

#include <cassert>
#include <iostream>
#include <fstream>
#include <string>

SignalHoundStream::SignalHoundStream(){
    device = -1;
    status = smNoError;
    setupDeviceUSB();
    words.resize(1000);
}

SignalHoundStream::~SignalHoundStream()= default;

void SignalHoundStream::setupDeviceUSB() {
    status = smOpenDevice(&device);
}

void SignalHoundStream::setupDeviceNetwork() {
    status = smOpenNetworkedDevice(&device, SM_ADDR_ANY, SM_DEFAULT_ADDR, SM_DEFAULT_PORT);
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
    words.clear();
    words.resize(wordCount);
    uint32_t *curr = words.data();

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
}

void SignalHoundStream::writeStream(const std::string &filename)
{
    if(!words.empty()) {
        std::ofstream out_file(filename, std::ios::out | std::ios::binary);
        if (out_file.is_open()) {
            out_file.write((char*)words.data(), words.size() * sizeof(uint32_t));
            out_file.close();
        } else {
            std::cerr << "Unable to open file: " << filename << std::endl;
        }
    }
}
