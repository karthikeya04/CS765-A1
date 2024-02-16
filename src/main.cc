
#include <memory>

#include "simulator.h"

int main() {
    auto network_params = std::make_shared<NetworkParams>();
    network_params->num_peers = 30;
    network_params->num_slow_peers = 17;
    network_params->num_low_cpu = 21;
    auto params = std::make_shared<Simulator::SimulationParams>();
    params->network_params = network_params;
    params->avg_blks_interarrival = 0.1;
    params->max_blocks = 100;
    params->Ttx = 200;
    auto sim = Simulator::CreatePtr(params);
    sim->Init_Network();
    sim->Run();
    sim->End();
}