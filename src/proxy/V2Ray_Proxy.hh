#pragma once

#include "Proxy.hh"

class V2Ray_Proxy : public Proxy
{
public:
  V2Ray_Proxy(const ProxyProfile& profile);

    virtual void wait_start() override;
    virtual void stop() override;
};
