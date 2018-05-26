#pragma once

#include "Proxy.hh"
#include "proxy/SS_Proxy.hh"

// SS_Proxy as Framework
class SSR_Proxy : public SS_Proxy
{
public:
    SSR_Proxy(const ProxyProfile& profile);

    virtual void wait_start() override;
    // Use SS_Proxy::dump_conf_file()

    // Use SS_Proxy::generate_conf_tmpfile()
    virtual void populate_conf_file(const ProxyProfile& profile,
                                    int fd) override;
};
