#pragma once

#include "base/Singleton.hh"

class ProxyBench : public Singleton<ProxyBench>
{
    friend class Singleton<ProxyBench>;

public:
    enum class exit_status
    {
        SUCCESS = 0,
        FAILURE = 1,
        // emmmm, over-designed
        BAD_CMDLINE,
        BAD_CONF_FILE,
    };

public:
    void init(int argc, char* argv[]);
    void run();
    void exit(enum exit_status);

private:
    ProxyBench() = default;

    void read_conf(int argc, char* argv[]);
};
