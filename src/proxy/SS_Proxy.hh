#pragma once

#include "Proxy.hh"

class SS_Proxy : public Proxy
{
public:
    SS_Proxy(const ProxyProfile& profile);

    virtual void wait_start() override;
    virtual void stop() override;
};
