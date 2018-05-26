#pragma once

#include <boost/any.hpp>
#include <memory>
#include <netinet/in.h>
#include <string>
// #include <map>
#include <unordered_map>

enum class ProxyType
{
    SS,
    SSR,
    V2Ray,
};

/* Requirements For ProxyProfile

---
Required Field,  Type
---
name          ,  std::string
type          ,  ProxyType
local_port    ,  std::string
---

 */
// typedef std::map<std::string, boost::any> ProxyProfile;
// unordered_map use TOO MUCH memory
typedef std::unordered_map<std::string, boost::any> ProxyProfile;

// Interface
class Proxy
{
public:
    virtual ~Proxy() = default;

    virtual void wait_start() = 0;
    virtual void stop();
    virtual void dump_config_file(const std::string& dir) = 0;

protected:
    Proxy() = default;

    pid_t _proxy_pid = 0;
};

// Factory
class Proxys
{
public:
    // although Proxy will not used by multiple threads...
    static std::shared_ptr<Proxy> NewProxy(ProxyType type,
                                           const ProxyProfile& profile);
};
