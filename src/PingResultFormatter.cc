#include "PingResultFormatter.hh"

#include <boost/any.hpp>
#include <sstream>
#include <string>

#include "Pinging.hh"
#include "Proxy.hh"

void
PingResultFormatter::init(std::string fmt)
{
    // TODO
}

std::string
PingResultFormatter::format(const PingResult& result)
{
    // TODO
    std::string delay =
        std::to_string(boost::any_cast<int>(result.at("http_delay")));

    std::string ntimeout =
        std::to_string(boost::any_cast<int>(result.at("http_ntimeout")));

    std::string nping =
        std::to_string(boost::any_cast<int>(result.at("http_nping")));

    std::string name = boost::any_cast<std::string>(
        boost::any_cast<ProxyProfile>(result.at("profile")).at("name"));

    return delay + " (" + ntimeout + "/"+ nping + ")  " + name;
}
