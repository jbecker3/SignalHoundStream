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
    if (argc != 10) {
        std::cerr << "Usage: " << argv[0] << " <usb|net> <center_freq> <sample_rate> <iq_bandwidth> <ref_level> <stream_id> <packet_size> <packet_count> <filename>\n";
        return 1;
    }

    std::string setupType = argv[1];
    if (setupType == "usb") {
        stream.setupDeviceUSB();
    } else if (setupType == "net") {
        stream.setupDeviceNetwork();
    } else if (setupType == "test") {
        //do nothing
    } else {
        std::cerr << "Invalid choice. Defaulting to usb.\n";
        stream.setupDeviceUSB();
    }


    IQ_Parameters parameters{};
    parameters.centerFreq = std::stod(argv[2]);
    parameters.sampleRate = std::atoi(argv[3]);
    parameters.iqBandwidth = std::stod(argv[4]);
    parameters.refLevel = std::stod(argv[5]);
    parameters.streamID = std::atoi(argv[6]);
    parameters.packetSize = std::atoi(argv[7]);
    int dataPacketCount = std::atoi(argv[8]);

    double sample_rate;
    if (setupType == "net")
    {
        sample_rate = 50.0 /float(parameters.sampleRate);
    }
    else
    {
        sample_rate = 200.0 /float(parameters.sampleRate);
    }

    std::cout << "Recording IQ Stream with these Parameters:" << std::endl;
    std::cout << "\tCenter Frequency: " << parameters.centerFreq / 1.0e6 << "MHz" << std::endl;
    std::cout << "\tSample Rate: " << sample_rate << "MS/s" << std::endl;
    std::cout << "\tIQ Bandwidth: " << parameters.iqBandwidth / 1.0e6 << "MHz" << std::endl;
    std::cout << "\tRef Level: " << parameters.refLevel << "dBm" << std::endl;
    std::cout << "\tStream ID: " << parameters.streamID << std::endl;
    std::cout << "\tPacket Size: " << parameters.packetSize << " words" << std::endl;
    std::cout << "\tPacket Count: " << dataPacketCount << " packets" << std::endl;

    if (setupType != "test") stream.getVRTPackets(dataPacketCount,parameters);

    std::string filename = argv[9];
    stream.writeStream(filename);
}