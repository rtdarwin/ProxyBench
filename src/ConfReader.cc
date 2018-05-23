#include "ConfReader.hh"

#include <algorithm>
#include <boost/program_options.hpp>
#include <cctype>
#include <exception>
#include <fstream>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <strings.h>
#include <vector>

#include "Conf.hh"
#include "Pinging.hh"
#include "Proxy.hh"
#include "ProxyBench.hh"
#include "SubscrParser.hh"
#include "config.h"

namespace po = boost::program_options;
using std::exception;
using std::cout;
using std::string;
using std::vector;

void
ConfReader::readConf(int argc, const char* const argv[])
{
    po::options_description visible("Generic Options");

    // positional options is hidden
    po::options_description hidden("Hidden Options");
    po::positional_options_description pos;

    // clang-format off
    visible.add_options()
        ("help,h", "show this help")
        ("version,V", "print version string")
        ("config,c", po::value<string>(),
           "specify configuration file")
        ("proxy,p", po::value<string>(),
           "ss, ssr, or v2ray")
        ("ping-protocol,P", po::value<string>()->default_value("http"),
           "icmp, tcp, or http")
        ("exec,e", po::value<string>(),
           "path/to/proxy.exec")
        ("num-threads,T", po::value<int>()->default_value(12),
           "nthreads")
        ("export-subscr-nodes,D"
           , po::value<string>()->implicit_value("./subscr-nodes")
           , "whether and where to export susbscription nodes")
        ("output-format,F", po::value<string>(),
           "// TODO")
        ("subscr-type,t", po::value<string>(),
           "ss-surge, ssr-url, or v2ray-vmess-file");
        ;

    hidden.add_options()("subscr-addr", po::value<vector<string>>());
    pos.add("subscr-addr", -1);
    // clang-format on

    /*  1. parse cmdline options */

    po::options_description all_options;
    all_options.add(visible).add(hidden);

    po::variables_map options;
    po::store(po::command_line_parser(argc, argv)
                  .options(all_options)
                  .positional(pos)
                  .run(),
              options);
    po::notify(options);

    /*  2. parse config file options */

    if (options.count("config")) {
        const string config_file = options["config"].as<string>();

        std::ifstream ifs(config_file.c_str());
        if (!ifs) {
            throw std::invalid_argument("Invalid `--config options`");
        }

        store(parse_config_file(ifs, all_options), options);
        notify(options);
    }

    /*  3. print help OR check required options */

    const std::function<void()>& print_help = [&argv, &visible]() {
        cout << "Usage: " << argv[0] << " [options] subscr-addr..."
             << "\n";
        cout << "\n" << visible << "\n";
    };

    if (options.count("help")) {
        print_help();
        ProxyBench::get()->exit(ProxyBench::exit_status::SUCCESS);
    }
    //
    if (options.count("version")) {
        cout << "proxy-bench  version " << PROXY_BENCH_VERSION << "\n";
        ProxyBench::get()->exit(ProxyBench::exit_status::SUCCESS);
    }

    // Required options
    if (!options.count("proxy") || !options.count("subscr-type")) {
        print_help();
        throw std::invalid_argument(
            "`--proxy, --subscr-type' options are required");
    }
    if (!options.count("subscr-addr")) {
        print_help();
        throw std::invalid_argument("at least one `subscr-addr' is required");
    }

    /*  4. react to options */

    Conf* conf = Conf::get();

    //
    if (options.count("proxy")) {
        const string& proxyType = options["proxy"].as<string>();

        if (0 == strcasecmp(proxyType.c_str(), "ss")) {
            conf->proxy_type = ProxyType::SS;
        } else if (0 == strcasecmp(proxyType.c_str(), "ssr")) {
            conf->proxy_type = ProxyType::SSR;
        } else if (0 == strcasecmp(proxyType.c_str(), "v2ray")) {
            conf->proxy_type = ProxyType::V2Ray;
        } else {
            throw std::invalid_argument("Invalid `--proxy' option");
        }
    }
    //
    if (options.count("exec")) {
        conf->exec = options["exec"].as<string>();
    }
    //
    if (options.count("protocol")) {
        const string& proto = options["protocol"].as<string>();

        if (0 == strcasecmp(proto.c_str(), "icmp")) {
            conf->protocol = PingProtocol::ICMP;
        } else if (0 == strcasecmp(proto.c_str(), "tcp")) {
            conf->protocol = PingProtocol::TCP;
        } else if (0 == strcasecmp(proto.c_str(), "http")) {
            conf->protocol = PingProtocol::HTTP;
        } else {
            throw std::invalid_argument("Invalid `--protocol` option");
        }
    }
    //
    if (options.count("num-threads")) {
        conf->nthreads = options["num-threads"].as<int>();
    }
    //
    if (options.count("output-format")) {
        conf->output_format = options["output-format"].as<string>();
    }
    //
    if (options.count("subscr-type")) {
        const string& subscr = options["subscr-type"].as<string>();

        if (0 == strcasecmp(subscr.c_str(), "ss-surge")) {
            conf->subscr_type = SubscrType::SS_SURGE;
        } else if (0 == strcasecmp(subscr.c_str(), "ssr-url")) {
            conf->subscr_type = SubscrType::SSR_URL;
        } else if (0 == strcasecmp(subscr.c_str(), "v2ray-vmess-file")) {
            conf->subscr_type = SubscrType::V2RAY_VMESS_FILE;
        } else {
            throw std::invalid_argument("Invalid `--subscr-type` option");
        }
    }
    //
    if (options.count("subscr-addr")) {
        conf->subscr_addrs = options["subscr-addr"].as<vector<string>>();
    }
}
