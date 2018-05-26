#pragma once

#include "Proxy.hh"

class V2Ray_Proxy : public Proxy
{
public:
    V2Ray_Proxy(const ProxyProfile& profile);

    virtual void wait_start() override;
    virtual void dump_config_file(const std::string& dir) override;
};
