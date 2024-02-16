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
    words.resize(1000);
}

SignalHoundStream::~SignalHoundStream()= default;

void SignalHoundStream::printUSBDevices()
{
    int serials[9];
    int deviceCount;
    smGetDeviceList(serials, &deviceCount);
    for (int i = 0; i < deviceCount; i++)
    {
        std::cout << "Device " << i << ": " << serials[i] << std::endl;
    }
}

void SignalHoundStream::setupDeviceUSB(int serial) {
    status = smOpenDeviceBySerial(&device,serial);
}

void SignalHoundStream::setupDeviceNetwork() {
    status = smOpenNetworkedDevice(&device, SM_ADDR_ANY, SM_DEFAULT_ADDR, SM_DEFAULT_PORT);
}

void SignalHoundStream::getVRTPackets(int packet_count,IQ_Parameters parameters) {
    assert(status == smNoError);

    // Set IQ parameters
    smSetIQCenterFreq(device, parameters.center_freq);
    smSetIQSampleRate(device, parameters.sample_rate);
    smSetIQBandwidth(device, smTrue, parameters.iq_bandwidth);
    smSetRefLevel(device, parameters.ref_level);

    // Set VRt parameters
    smSetVrtStreamID(device, parameters.stream_id);
    smSetVrtPacketSize(device, parameters.packet_size);

    // Configure
    status = smConfigure(device, smModeIQStreaming); // VRT mode
    assert(status == smNoError);

    // Allocate memory
    uint32_t context_word_count;
    status = smGetVrtContextPktSize(device, &context_word_count);
    assert(status == smNoError);

    uint16_t samples_per_packet;
    uint32_t data_word_count;
    status = smGetVrtPacketSize(device, &samples_per_packet, &data_word_count);
    assert(status == smNoError);

    uint32_t word_count = context_word_count + data_word_count * packet_count;
    words.clear();
    words.resize(word_count);
    uint32_t *curr = words.data();

    // Get context packet
    uint32_t actual_context_word_count;
    status = smGetVrtContextPkt(device, curr, &actual_context_word_count);
    assert(status == smNoError);
    assert(actual_context_word_count == context_word_count);
    curr += context_word_count;

    // Get data packets
    uint32_t actual_data_word_count;
    status = smGetVrtPackets(device, curr, &actual_data_word_count, packet_count, smFalse);
    assert(status == smNoError);
    assert(actual_data_word_count == data_word_count * packet_count);

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
