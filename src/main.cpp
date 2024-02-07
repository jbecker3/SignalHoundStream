//
// Created by Jonathan Becker on 2/6/2024.
//

#include "SignalHoundStream.h"

#include <iostream>
#include <cstdlib>
#include <string>

SignalHoundStream stream;

int main(int argc, char* argv[])
{
    // Check if the correct number of arguments is provided
    if (argc != 9) {
        std::cerr << "Usage: " << argv[0] << " <usb|net> <center_freq> <sample_rate> <iq_bandwidth> <ref_level> <stream_id> <packet_size> <packet_count>\n";
        return 1;
    }

    std::string setupType = argv[1];
    if (setupType == "usb") {
        stream.setupDeviceUSB();
    } else if (setupType == "net") {
        stream.setupDeviceNetwork();
    } else {
        std::cerr << "Invalid choice. Defaulting to usb.\n";
        stream.setupDeviceUSB();
    }


    IQ_Parameters parameters;
    parameters.centerFreq = std::stod(argv[2]);
    parameters.sampleRate = std::atoi(argv[3]);
    parameters.iqBandwidth = std::stod(argv[4]);
    parameters.refLevel = std::atoi(argv[5]);
    parameters.streamID = std::atoi(argv[6]);
    parameters.packetSize = std::atoi(argv[7]);
    int dataPacketCount = std::atoi(argv[8]);
    stream.getVRTPackets(dataPacketCount,parameters);
}