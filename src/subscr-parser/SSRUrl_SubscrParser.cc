#include "subscr-parser/SSRUrl_SubscrParser.hh"

#include <iostream>
#include <list>
#include <regex>
#include <sstream>
#include <string>

#include "HttpClient.hh"
#include "Proxy.hh"
#include "SyncDnsResolver.hh"
#include "base/Strings.hh"
#include "third-party/cppcodec/base64_url_unpadded.hpp"

using std::string;
using namespace cppcodec;
using namespace proxybench;

namespace {

/*  <host>:<port>:<protocol>:<method>:<obfs>:<password_b64>
    /?obfsparam=<obfs_param_b64>
      &protoparam=<proto_param_b64>
      &remarks=<remarks_b64>
      &group=<group_b64>
 */
const std::regex SSR_PROFILE_PATTERN = std::regex(
    R"raw(^([_[:alnum:].-]+):([[:digit:]]+):([_[:alnum:]-]+):([_[:alnum:]-]+):([_[:alnum:]-]+):([_[:alnum:]/-]+)/\?obfsparam=([_[:alnum:]-]*)&protoparam=([_[:alnum:]-]*)&remarks=([_[:alnum:]-]*)&group=([_[:alnum:]-]*)$)raw");

} // namespace {

void
SSRUrl_SubscrParser::parse(const std::string& subscr_addr,
                           std::list<ProxyProfile>* profiles)
{
    string nodes_base64_text;
    HttpClient httpclient;

    std::cout << "Retrieving " << subscr_addr << "......" << std::endl; // DEBUG
    // The fetching of subscription is time-consuming ...
    httpclient.timeout(60).wait_get(subscr_addr, &nodes_base64_text);

    string nodes_text = base64_url_unpadded::decode<string>(nodes_base64_text);
    std::stringstream nodes_stream(nodes_text);

    string line;
    while (std::getline(nodes_stream, line)) {
        if (line.empty())
            continue;

        line = base64_url_unpadded::decode<string>(line.c_str() + 6,
                                                   line.length() - 6);

        std::smatch backrefs;
        ProxyProfile profile;
        if (std::regex_match(line, backrefs, SSR_PROFILE_PATTERN)) {
            profile["type"] = ProxyType::SSR;
            profile["name"] =
                base64_url_unpadded::decode<string>(Strings::trim(backrefs[9]));
            profile["server"] =
                SyncDnsResolver::get()->resolve(Strings::trim(backrefs[1]));
            profile["server_port"] = Strings::trim(backrefs[2]);
            profile["protocol"] = Strings::trim(backrefs[3]);
            profile["method"] = Strings::trim(backrefs[4]);
            profile["obfs"] = Strings::trim(backrefs[5]);
            profile["password"] =
                base64_url_unpadded::decode<string>(Strings::trim(backrefs[6]));
            profile["obfs_param"] =
                base64_url_unpadded::decode<string>(Strings::trim(backrefs[7]));
            profile["protocol_param"] =
                base64_url_unpadded::decode<string>(Strings::trim(backrefs[8]));
            profile["group"] = base64_url_unpadded::decode<string>(
                Strings::trim(backrefs[10]));

            profiles->push_back(std::move(profile));
        }
    }
}
