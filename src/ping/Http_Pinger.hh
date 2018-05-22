#pragma once

#include "Pinging.hh"

class Http_Pinger : public Pinger
{
public:
    Http_Pinger() = default;
    virtual PingResult wait_ping(struct sockaddr_in proxyAddr) override;
};
