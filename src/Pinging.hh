#pragma once

#include <memory>
#include <netinet/in.h>
#include <unordered_map>
#include <boost/any.hpp>

enum class PingProtocol
{
    ICMP,
    TCP,
    HTTP,
};

typedef std::unordered_map<std::string, boost::any> PingResult;

// Interface
class Pinger
{
public:
    virtual PingResult wait_ping(struct sockaddr_in proxyAddr) = 0;

protected:
    Pinger() = default;
};

// Factory
class Pingers
{
public:
    static std::shared_ptr<Pinger> NewPinger(PingProtocol proto);
};
