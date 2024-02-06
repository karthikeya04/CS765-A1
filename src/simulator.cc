
#include "simulator.h"

std::mt19937_64 Simulator::rng(std::chrono::steady_clock::now().time_since_epoch().count());

Simulator::Ptr Simulator::CreatePtr(std::shared_ptr<SimulationParams> params) {
    return make_shared<Simulator>(std::move(params));
}

Simulator::Simulator(std::shared_ptr<SimulationParams> params) 
    : network_(std::make_shared<Network>(params->network_params)),
    params_(std::move(params)) {}

void Simulator::Schedule(Event::Ptr event, SimTime delay = 0.0) {
    queued_events_.emplace(now_ + delay, event);
}

bool Simulator::Step() {
    if(queued_events_.empty())
        return false;
    auto qe = queued_events_.top();
    queued_events_.pop();
    now_ = qe.time;
    auto event = qe.event;
    event->Process();
    return true;
}

void Simulator::Run() {
    while( Step() ) {}
}

Simulator::QueuedEvent::QueuedEvent(SimTime time, Event::Ptr event) 
    : time(time), event(event) {}

bool Simulator::QueuedEvent::operator<(const QueuedEvent &other) const {
    return time > other.time;
}

