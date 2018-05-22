#pragma once

#include <boost/any.hpp>
#include <memory>
#include <netinet/in.h>
#include <string>
#include <unordered_map>

enum class ProxyType
{
    SS,
    SSR,
    V2Ray,
};

typedef std::unordered_map<std::string, boost::any> ProxyProfile;

// Interface
class Proxy
{
public:
    virtual void wait_start() = 0;
    virtual void stop() = 0;
    sockaddr_in addr();

protected:
    Proxy() = default;
};

// Factory
class Proxys
{
public:
    // although Proxy will not used by multiple threads...
    static std::shared_ptr<Proxy> NewProxy(ProxyType type,
                                           const ProxyProfile& profile);
};
