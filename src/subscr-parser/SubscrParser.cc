#include "SubscrParser.hh"

#include "subscr-parser/SSRUrl_SubscrParser.hh"
#include "subscr-parser/SSSurge_SubscrParser.hh"
#include "subscr-parser/V2RayVmessFile_SubscrParser.hh"

using std::shared_ptr;
using std::make_shared;

shared_ptr<SubscrParser>
SubscrParsers::NewParser(SubscrType type)
{
    shared_ptr<SubscrParser> parser;

    switch (type) {
        case SubscrType::SS_SURGE:
            parser = make_shared<SSSurge_SubscrParser>();
            break;
        case SubscrType::SSR_URL:
            parser = make_shared<SSRUrl_SubscrParser>();
            break;
        case SubscrType::V2RAY_VMESS_FILE:
            parser = make_shared<V2RayVmessFile_SubscrParser>();
            break;
    }

    return parser;
}
