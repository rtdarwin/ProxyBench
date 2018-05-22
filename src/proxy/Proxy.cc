#include "Proxy.hh"

#include "proxy/SSR_Proxy.hh"
#include "proxy/SS_Proxy.hh"
#include "proxy/V2Ray_Proxy.hh"

using std::shared_ptr;
using std::make_shared;

sockaddr_in
Proxy::addr()
{
    // TODO
}

shared_ptr<Proxy>
Proxys::NewProxy(ProxyType type, const ProxyProfile& profile)
{
    shared_ptr<Proxy> proxy;

    switch (type) {
        case ProxyType::SS:
            proxy = make_shared<SS_Proxy>(profile);
            break;
        case ProxyType::SSR:
            proxy = make_shared<SSR_Proxy>(profile);
            break;
        case ProxyType::V2Ray:
            proxy = make_shared<V2Ray_Proxy>(profile);
            break;
    }

    return proxy;
}
