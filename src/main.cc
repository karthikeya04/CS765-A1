
#include <jsoncpp/json/json.h>

#include <fstream>
#include <iostream>
#include <memory>

#include "simulator.h"

int main() {
    std::ifstream args_file("args.json", std::ifstream::binary);
    if (!args_file.is_open()) {
        std::cerr << "Error: Unable to open file\n";
        return 1;
    }
    Json::Value args;
    args_file >> args;

    auto network_params = std::make_shared<NetworkParams>();
    network_params->num_peers = args["num_peers"].asInt();
    network_params->num_slow_peers = args["num_slow_peers"].asInt();
    network_params->num_low_cpu = args["num_low_cpu"].asInt();
    network_params->T_dij = args["Tdij"].asDouble();
    network_params->hashPower_self1 = 
        args["hashing_power_selfish_1"].asDouble();
    network_params->hashPower_self2 = 
        args["hashing_power_selfish_2"].asDouble();
    
    if (network_params->hashPower_self1 + 
        network_params->hashPower_self2 >= 1) {
        std::cerr << "Error: invalid hashing power combination given\n";
        return 1;
    }

    auto simulator_params = std::make_shared<Simulator::SimulationParams>();
    simulator_params->network_params = network_params;
    simulator_params->avg_blks_interarrival =
        args["avg_blks_interarrival"].asDouble();
    simulator_params->max_blocks = args["max_blocks"].asInt();
    simulator_params->Ttx = args["Ttx"].asDouble();

    auto sim = Simulator::CreatePtr(simulator_params);
    sim->Init_Network();
    sim->Run();
    sim->End();
}