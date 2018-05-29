#include "subscr-parser/SSSurge_SubscrParser.hh"

#include <iostream>
#include <list>
#include <regex>
#include <sstream>

#include "Conf.hh"
#include "HttpClient.hh"
#include "Proxy.hh"
#include "SyncDnsResolver.hh"
#include "base/Strings.hh"

using std::string;
using namespace proxybench;

namespace {

/*  <NODE-NAME> = custom,<host>,<port>,<method>,<password>,xxxmodule,<obfs> */
const std::regex SS_PROFILE_PATTERN = std::regex(
    R"raw(^(.+)=\s*custom,([_[:alnum:].-]+),([[:digit:]]+),([_[:alnum:]-]+),(\S+),.*$)raw");

const std::regex SIMPLE_OBFS_PATTERN = std::regex(
    R"raw(^.+(obfs=.+)\s*$)raw");

} // namespace {

void
SSSurge_SubscrParser::parse(const std::string& subscr_addr,
                            std::list<ProxyProfile>* profiles)
{
    std::string nodes_text;
    HttpClient httpclient;

    {
        if (Conf::get()->verbose) {
            std::cout << "Retrieving " << subscr_addr << "......" << std::endl;
        }
    }
    // The fetching of surge subscription is time-consuming ...
    httpclient.timeout(60).wait_get(subscr_addr, &nodes_text);

    if (nodes_text.size() > 18000) {
        // 18000 bytes is enough for [Proxy] block (maybe
        nodes_text.resize(18000);
    }
    std::stringstream nodes_stream(nodes_text);

    string line;
    bool inProxyBlock = false;
    bool lastMatch = false;
    while (std::getline(nodes_stream, line)) {
        // line = trim(line);
        if (line.empty())
            continue;

        if (line[0] == '[' && line[line.length() - 1] == ']')
            inProxyBlock = (line == "[Proxy]");

        if (inProxyBlock) {
            // std::cout << "Yes, in [Proxy]" << std::endl; // DEBUG
            std::smatch backrefs;
            ProxyProfile profile;
            if (std::regex_match(line, backrefs, SS_PROFILE_PATTERN)) {
                profile["type"] = ProxyType::SS;
                // profile["group"] = group;
                profile["name"] = Strings::trim(backrefs[1]);
                profile["server"] =
                    SyncDnsResolver::get()->resolve(Strings::trim(backrefs[2]));
                profile["server_port"] = Strings::trim(backrefs[3]);
                profile["password"] = Strings::trim(backrefs[5]);
                profile["method"] = Strings::trim(backrefs[4]);

                if (std::regex_match(line, backrefs, SIMPLE_OBFS_PATTERN)) {
                    profile["plugin_option"] =
                        Strings::trim(Strings::replace(backrefs[1], ',', ';'));
                }

                lastMatch = true;
                profiles->push_back(std::move(profile));
            } else {
                if (lastMatch) {
                    break;
                }
            }
        }
    }
}
