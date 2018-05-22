#pragma once

#include <string>
#include <vector>

#include "base/Singleton.hh"

enum class ProxyType;
enum class PingProtocol;
enum class SubscrType;

class Conf : public Singleton<Conf>
{
    friend class Singleton<Conf>;

public:
    ProxyType proxy_type;
    std::string exec;
    PingProtocol protocol;
    int nthreads;
    std::string output_format;
    SubscrType subscr_type;
    std::vector<std::string> subscr_addrs;

private:
    Conf() = default;
};
