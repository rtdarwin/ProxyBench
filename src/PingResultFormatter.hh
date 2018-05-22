#pragma once

#include <string>

#include "Pinging.hh"

class PingResultFormatter
{
public:
    PingResultFormatter() = default;

    void init(std::string fmt);
    std::string format(const PingResult&);
};
