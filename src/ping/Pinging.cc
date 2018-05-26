#include "Pinging.hh"

#include "ping/Http_Pinger.hh"
#include "ping/Icmp_Pinger.hh"
#include "ping/Tcp_Pinger.hh"

using std::shared_ptr;
using std::make_shared;

bool
Pinger::require_proxy()
{
    return false;
}

Pinger&
Pinger::socks5_proxy(const std::string& proxy_addr)
{
    _socks5_proxy = proxy_addr;
    return *this;
}

shared_ptr<Pinger>
Pingers::NewPinger(PingProtocol proto)
{
    shared_ptr<Pinger> pinger;

    switch (proto) {
        case PingProtocol::ICMP:
            pinger = make_shared<Icmp_Pinger>();
            break;
        case PingProtocol::TCP:
            pinger = make_shared<Tcp_Pinger>();
            break;
        case PingProtocol::HTTP:
            pinger = make_shared<Http_Pinger>();
            break;
    }

    return pinger;
}
