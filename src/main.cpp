//
// Created by Jonathan Becker on 2/6/2024.
//

#include "SignalHoundStream.h"

#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <omp.h>

int main(int argc, char* argv[])
{
    // Check if the correct number of arguments is provided
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_filename>" << std::endl;
        return 1;
    }

    std::string input_filename = argv[1];
    std::ifstream input_file(input_filename);
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

        std::vector<SignalHoundStream> streams;
        std::vector<IQ_Parameters> stream_parameters;
        std::vector<int> packet_counts;
        std::vector<std::string> output_filenames;

        //Setup streams
        for(const auto& stream_data : streams_data)
        {
            SignalHoundStream stream;
            std::string connection_type = stream_data["connection_type"];

            IQ_Parameters parameters{};
            int packet_count;
            int serial;
            std::string output_filename;
            if (stream_data.contains("serial") && 
                stream_data.contains("center_freq") &&
                stream_data.contains("sample_rate") &&
                stream_data.contains("iq_bandwidth") &&
                stream_data.contains("ref_level") &&
                stream_data.contains("stream_id") &&
                stream_data.contains("packet_size") &&
                stream_data.contains("packet_count") &&
                stream_data.contains("output_file")) {
                serial = stream_data["serial"];
                parameters.center_freq = stream_data["center_freq"];
                parameters.sample_rate = stream_data["sample_rate"];
                parameters.iq_bandwidth = stream_data["iq_bandwidth"];
                parameters.ref_level = stream_data["ref_level"];
                parameters.stream_id = stream_data["stream_id"];
                parameters.packet_size = stream_data["packet_size"];
                packet_count = stream_data["packet_count"];
                output_filename = stream_data["output_file"];
            } else {
                std::cerr << "Input file does not contain all necessary fields." << std::endl;
                return 1;
            }

            if (connection_type == "usb") {
                stream.setupDeviceUSB(serial);
            }
            else if (connection_type == "net") {
                stream.setupDeviceNetwork();
            }
            else if (connection_type == "test") {
                //do nothing
            }
            else {
                std::cerr << "Invalid connection type. Defaulting to usb.\n";
                stream.setupDeviceUSB(serial);
            }

            double sample_rate;
            if (connection_type == "net") sample_rate = 50.0 / float(parameters.sample_rate);
            else sample_rate = 200.0 / float(parameters.sample_rate);

            std::cout << "Recording IQ Stream with these Parameters:" << std::endl;
            std::cout << "\tSerial: " << serial << std::endl;
            std::cout << "\tCenter Frequency: " << parameters.center_freq / 1.0e6 << "MHz" << std::endl;
            std::cout << "\tSample Rate: " << sample_rate << "MS/s" << std::endl;
            std::cout << "\tIQ Bandwidth: " << parameters.iq_bandwidth / 1.0e6 << "MHz" << std::endl;
            std::cout << "\tRef Level: " << parameters.ref_level << "dBm" << std::endl;
            std::cout << "\tStream ID: " << parameters.stream_id << std::endl;
            std::cout << "\tPacket Size: " << parameters.packet_size << " words" << std::endl;
            std::cout << "\tPacket Count: " << packet_count << " packets" << std::endl;
            std::cout << "\tOutput File: " << output_filename << std::endl;

            streams.push_back(stream);
            stream_parameters.push_back(parameters);
            packet_counts.push_back(packet_count);
            output_filenames.push_back(output_filename);
        }

        //Collect Packets and write to file
        #pragma omp parallel for
        for(int i = 0; i < streams.size(); i++)
        {
            std::cout << "Started Streaming device " << i << std::endl;
            if (streams_data[i]["connection_type"] != "test") {
                streams[i].getVRTPackets(packet_counts[i], stream_parameters[i]);
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::seconds(10));
            }
            streams[i].writeStream(output_filenames[i]);
            std::cout << "Finished Streaming device " << i << std::endl;
        }
    }
    else
    {
        std::cerr << "Problem reading input file." << std::endl;
        return 1;
    }
}