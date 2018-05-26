#pragma once

#include <list>
#include <memory>
#include <string>

#include "Proxy.hh"

enum class SubscrType
{
    SS_SURGE,
    SSR_URL,
    V2RAY_VMESS_FILE,
};

// Interface
class SubscrParser
{
public:
    virtual ~SubscrParser() = default;

    virtual void parse(const std::string& subscr_addr,
                       std::list<ProxyProfile>* profiles) = 0;

protected:
    SubscrParser() = default;
};

// Factory
class SubscrParsers
{
public:
    static std::shared_ptr<SubscrParser> NewParser(SubscrType type);
};
