#pragma once

#include <boost/any.hpp>
#include <memory>
#include <netinet/in.h>
#include <string>

class HttpClient
{
public:
    HttpClient();
    ~HttpClient();

    HttpClient& socks5_proxy(const std::string& proxy_addr);
    HttpClient& dns_resolver(const std::string& resolver_addr);
    HttpClient& timeout(int seconds);
    HttpClient& just_ping(bool);
    // std::string(text) or int(delay)
    void wait_get(const std::string& url, std::string* result);

private:
    // hide curl by using PIMPL
    struct Impl;
    std::unique_ptr<Impl> _pImpl;
};
