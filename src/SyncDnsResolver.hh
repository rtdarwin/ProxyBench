#pragma once

#include <string>

#include "base/Singleton.hh"

class SyncDnsResolver : public Singleton<SyncDnsResolver>
{
    friend class Singleton<SyncDnsResolver>;

public:
    std::string resolve(const std::string& host);
};
