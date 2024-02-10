
#include <memory>

#include "simulator.h"

int main() {
    auto network_params = std::make_shared<NetworkParams>();
    network_params->num_peers = 20;
    network_params->num_slow_peers = 10;
    network_params->num_low_cpu = 7;
    auto params = std::make_shared<Simulator::SimulationParams>();
    params->network_params = network_params;
    auto sim = Simulator::CreatePtr(params);
    sim->Init_Network();
    sim->Run();
    sim->ExportData();
}