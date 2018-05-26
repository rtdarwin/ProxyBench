#pragma once

#include <boost/any.hpp>
#include <memory>
#include <netinet/in.h>
// #include <map>
#include <unordered_map>

enum class PingProtocol
{
    ICMP,
    TCP,
    HTTP,
};

/* Requirements For PingResult

---
Required Field,  Type
---
icmp_delay    ,  int(ms)
icmp_nping    ,  int
icmp_ntimeout ,  int

tcp_delay     ,  int(ms)
tcp_nping     ,  int
tcp_ntimeout  ,  int

http_delay    ,  int(ms)
http_nping    ,  int
http_ntimeout ,  int

profile       ,  ProxyProfile
---

 */
// typedef std::map<std::string, boost::any> PingResult;
// unordered_map use TOO MUCH memory
typedef std::unordered_map<std::string, boost::any> PingResult;

// Interface
class Pinger
{
public:
    virtual ~Pinger() = default;

    virtual bool require_proxy();
    virtual Pinger& socks5_proxy(const std::string& proxy_addr);
    virtual void wait_ping(PingResult* result) = 0;

protected:
    Pinger() = default;

    std::string _socks5_proxy;
};

// Factory
class Pingers
{
public:
    static std::shared_ptr<Pinger> NewPinger(PingProtocol proto);
};
