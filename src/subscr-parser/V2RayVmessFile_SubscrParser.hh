#pragma once

#include "SubscrParser.hh"

class V2RayVmessFile_SubscrParser : public SubscrParser
{
public:
    V2RayVmessFile_SubscrParser() = default;

    virtual void parse(const std::string& subscr_file,
                       std::list<ProxyProfile>* profiles) override;
};
