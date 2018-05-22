#pragma once

#include "Pinging.hh"

class Tcp_Pinger : public Pinger
{
public:
    Tcp_Pinger() = default;
    virtual PingResult wait_ping(struct sockaddr_in proxyAddr) override;
};
