#pragma once

#include "Pinging.hh"

class Tcp_Pinger : public Pinger
{
public:
    Tcp_Pinger() = default;

    virtual bool require_proxy() override;
    virtual void wait_ping(PingResult*) override;
};
