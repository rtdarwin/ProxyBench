#pragma once

#include <list>

#include "SubscrParser.hh"

class SSSurge_SubscrParser : public SubscrParser
{
public:
    SSSurge_SubscrParser() = default;

    virtual void parse(const std::string& subscr_addr,
                       std::list<ProxyProfile>* profiles) override;
};
