//
// Created by Jonathan Becker on 2/6/2024.
//

#include "SignalHoundStream.h"

#include <iostream>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>

int main(int argc, char* argv[])
{
    // Check if the correct number of arguments is provided
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_filename>" << std::endl;
        return 1;
    }

    std::string input_filename = argv[1];
    std::ifstream input_file(input_filename);
    SignalHoundStream stream;
    nlohmann::json json_input;

    if(!input_file.is_open())
    {
        std::cerr << "Could not open input file." << std::endl;
        return 1;
    }

    input_file >> json_input;
    input_file.close();

    if(json_input.contains("stream"))
    {
        auto& streams_data = json_input["stream"];
        for(const auto& stream_data : streams_data)
        {
            std::string connection_type = stream_data["connection_type"];
            if (connection_type == "usb") {
                stream.setupDeviceUSB();
            } else if (connection_type == "net") {
                stream.setupDeviceNetwork();
            } else if (connection_type == "test") {
                //do nothing
            } else {
                std::cerr << "Invalid connection type. Defaulting to usb.\n";
                stream.setupDeviceUSB();
            }

            IQ_Parameters parameters{};
            int packet_count = 0;
            std::string output_filename;
            if(stream_data.contains("center_freq") &&
               stream_data.contains("sample_rate") &&
               stream_data.contains("iq_bandwidth") &&
               stream_data.contains("ref_level") &&
               stream_data.contains("stream_id") &&
               stream_data.contains("packet_size") &&
               stream_data.contains("packet_count") &&
               stream_data.contains("output_file"))
            {
                parameters.center_freq = stream_data["center_freq"];
                parameters.sample_rate = stream_data["sample_rate"];
                parameters.iq_bandwidth = stream_data["iq_bandwidth"];
                parameters.ref_level = stream_data["ref_level"];
                parameters.stream_id = stream_data["stream_id"];
                parameters.packet_size = stream_data["packet_size"];
                packet_count = stream_data["packet_count"];
                output_filename = stream_data["output_file"];
            }
            else
            {
                std::cerr << "Input file does not contain all necessary fields." << std::endl;
                return 1;
            }

            double sample_rate;
            if (connection_type == "net") sample_rate = 50.0 /float(parameters.sample_rate);
            else sample_rate = 200.0 /float(parameters.sample_rate);

            std::cout << "Recording IQ Stream with these Parameters:" << std::endl;
            std::cout << "\tCenter Frequency: " << parameters.center_freq / 1.0e6 << "MHz" << std::endl;
            std::cout << "\tSample Rate: " << sample_rate << "MS/s" << std::endl;
            std::cout << "\tIQ Bandwidth: " << parameters.iq_bandwidth / 1.0e6 << "MHz" << std::endl;
            std::cout << "\tRef Level: " << parameters.ref_level << "dBm" << std::endl;
            std::cout << "\tStream ID: " << parameters.stream_id << std::endl;
            std::cout << "\tPacket Size: " << parameters.packet_size << " words" << std::endl;
            std::cout << "\tPacket Count: " << packet_count << " packets" << std::endl;
            std::cout << "\tOutput File: " << output_filename << std::endl;

            if (connection_type != "test") stream.getVRTPackets(packet_count,parameters);
            stream.writeStream(output_filename);
        }
    }
    else
    {
        std::cerr << "Problem reading input file." << std::endl;
        return 1;
    }
}