#include "subscr-parser/V2RayVmessFile_SubscrParser.hh"

#include <iostream>
#include <list>
#include <regex>
#include <sstream>
#include <string>

#include "HttpClient.hh"
#include "Proxy.hh"
#include "SyncDnsResolver.hh"
#include "base/Strings.hh"
#include "third-party/cppcodec/base64_rfc4648.hpp"

using std::string;
using namespace cppcodec;
using namespace proxybench;

namespace {

/*  
 */
const std::regex SSR_PROFILE_PATTERN = std::regex(
    R"raw()raw");

} // namespace {

void
V2RayVmessFile_SubscrParser::parse(const std::string& subscr_file,
                                   std::list<ProxyProfile>* profiles)
{
    // TODO
}
