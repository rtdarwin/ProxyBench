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
    {
        std::smatch backrefs;
        if (std::regex_match(host, backrefs, NUMBERICHOST_PATTERN)) {
            return host;
        }
    }

    struct addrinfo hints;
    struct addrinfo* answer = nullptr;
    char addrp[INET6_ADDRSTRLEN + 1] = { 0 };

    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;

    if (getaddrinfo(host.c_str(), nullptr, &hints, &answer) ||
        answer == nullptr) {
        freeaddrinfo(answer);
        throw std::runtime_error("DNS lookup for " + host + " failed.");
    }

    inet_ntop(answer->ai_family,
              &(((struct sockaddr_in*)(answer->ai_addr))->sin_addr), addrp,
              INET_ADDRSTRLEN);

    freeaddrinfo(answer);

    return addrp;
}
