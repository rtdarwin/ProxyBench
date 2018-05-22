#pragma once

#include "Pinging.hh"

class Icmp_Pinger : public Pinger
{
public:
    Icmp_Pinger() = default;
    virtual PingResult wait_ping(struct sockaddr_in proxyAddr) override;
};
