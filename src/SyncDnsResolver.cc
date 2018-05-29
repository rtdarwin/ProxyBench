#include "SyncDnsResolver.hh"

#include <arpa/inet.h>
#include <exception>
#include <netdb.h>
#include <regex>
#include <stdexcept>
#include <string>

std::regex NUMBERICHOST_PATTERN("^[[:digit:].]+$");

std::string
SyncDnsResolver::resolve(const std::string& host)
{
    struct addrinfo* answer = nullptr;
    char addrp[INET6_ADDRSTRLEN + 1] = { 0 };

    if (getaddrinfo(host.c_str(), nullptr, nullptr, &answer) ||
        answer == nullptr) {
        freeaddrinfo(answer);
        throw std::runtime_error("DNS lookup for " + host + " failed.");
    }

    void* addr =
        (answer->ai_addr)->sa_family == AF_INET
            ? (void*)&((struct sockaddr_in*)(answer->ai_addr))->sin_addr
            : (void*)&((struct sockaddr_in6*)(answer->ai_addr))->sin6_addr;

    inet_ntop(answer->ai_family, addr, addrp, INET6_ADDRSTRLEN);

    freeaddrinfo(answer);

    return addrp;
}
