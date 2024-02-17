
#include "simulator.h"

#include "debug.h"
#include "network.h"

std::mt19937_64 Simulator::rng(
    std::chrono::steady_clock::now().time_since_epoch().count());

RealUniformDistr Network::Link::prop_delay_distr(MIN_PROPAGATION_DELAY,
                                                 MAX_PROPAGATION_DELAY);
double Network::Link::propagation_delay =
    Network::Link::prop_delay_distr(Simulator::rng);

//-----------------------------------------------------------------------------

Simulator::Ptr Simulator::CreatePtr(std::shared_ptr<SimulationParams> params) {
    return std::make_shared<Simulator>(std::move(params));
}

//-----------------------------------------------------------------------------

Simulator::Simulator(std::shared_ptr<SimulationParams> params)
    : params_(std::move(params)) {}

//-----------------------------------------------------------------------------

void Simulator::Schedule(EventPtr event, double delay) {
    queued_events_.emplace(now_ + delay, event);
}

//-----------------------------------------------------------------------------

bool Simulator::Step() {
    if (queued_events_.empty()) return false;
    auto qe = queued_events_.top();
    queued_events_.pop();
    now_ = qe.time;
    auto event = qe.event;
    if (!event->isAborted()) event->Process();
    return true;
}

//-----------------------------------------------------------------------------

void Simulator::Run() {
    while (Step()) {
    }
}

//-----------------------------------------------------------------------------

void Simulator::End() { network_->End(); }

//-----------------------------------------------------------------------------

void Simulator::Init_Network() {
    network_ =
        std::make_shared<Network>(params_->network_params, shared_from_this());
    network_->Init();
}

//-----------------------------------------------------------------------------

double Simulator::GetTtx() { return params_->Ttx; }
double Simulator::Now() { return now_; }
//-----------------------------------------------------------------------------

Simulator::QueuedEvent::QueuedEvent(double time, EventPtr event)
    : time(time), event(event) {}

//-----------------------------------------------------------------------------

bool Simulator::QueuedEvent::operator<(const QueuedEvent &other) const {
    return time > other.time;
}

//-----------------------------------------------------------------------------
