
#include <memory>

#include "simulator.h"

int main()
{
    auto network_params = std::make_shared<NetworkParams>();
    network_params->num_peers = 5000;
    auto params = std::make_shared<Simulator::SimulationParams>();
    params->network_params = network_params;
    auto sim = Simulator::CreatePtr(params);
    sim->Run();
}