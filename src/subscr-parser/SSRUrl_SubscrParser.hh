#pragma once

#include "SubscrParser.hh"

class SSRUrl_SubscrParser : public SubscrParser
{
public:
    SSRUrl_SubscrParser() = default;

    virtual void parse(const std::string& subscr_addr,
                       std::vector<ProxyProfile>* profiles) override;
};
