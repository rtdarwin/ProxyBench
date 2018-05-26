#pragma once

#include "Proxy.hh"

class V2Ray_Proxy : public Proxy
{
public:
    V2Ray_Proxy(const ProxyProfile& profile);
    ~V2Ray_Proxy();

    virtual void wait_start() override;
    virtual void dump_config_file(const std::string& dir) override;

    void generate_conf_tmpfile();
    void populate_conf_file(const ProxyProfile& profile, int fd);

protected:
    const ProxyProfile& _profile;
    int _profile_tmpfile_fd = 0;
    char _profile_tmpfile[40] = "/tmp/proxybench-v2ray-profile-XXXXXX";
};
