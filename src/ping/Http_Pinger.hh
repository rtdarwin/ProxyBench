#pragma once

#include <string>

#include "Pinging.hh"

class Http_Pinger : public Pinger
{
public:
    Http_Pinger() = default;

    virtual bool require_proxy() override;
    virtual void wait_ping(PingResult*) override;
};
