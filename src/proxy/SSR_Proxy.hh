#pragma once

#include "Proxy.hh"

class SSR_Proxy : public Proxy
{
public:
    SSR_Proxy(const ProxyProfile& profile);

    virtual void wait_start() override;
    virtual void stop() override;
};
