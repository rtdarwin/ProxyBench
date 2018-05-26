#pragma once

#include "Pinging.hh"

class Icmp_Pinger : public Pinger
{
public:
    Icmp_Pinger() = default;

    virtual bool require_proxy() override;
    virtual void wait_ping(PingResult*) override;
};
