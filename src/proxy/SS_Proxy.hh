#pragma once

#include "Proxy.hh"

class SS_Proxy : public Proxy
{
public:
    SS_Proxy(const ProxyProfile& profile);
    virtual ~SS_Proxy();

    virtual void wait_start() override;
    virtual void dump_config_file(const std::string& dir) override;

    virtual void generate_conf_tmpfile();
    virtual void populate_conf_file(const ProxyProfile& profile, int fd);

protected:
    const ProxyProfile& _profile;
    int _profile_tmpfile_fd = 0;
    char _profile_tmpfile[40] = "/tmp/proxybench-ss(r)-profile-XXXXXX";
};
