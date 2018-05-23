#include "ProxyBench.hh"

#include <semaphore.h>

#include <algorithm>
#include <boost/any.hpp>
#include <exception>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <list>
#include <vector>

#include "Conf.hh"
#include "ConfReader.hh"
#include "PingResultFormatter.hh"
#include "Pinging.hh"
#include "Proxy.hh"
#include "SubscrParser.hh"

using std::cout;
using std::list;
using std::vector;
using std::string;

void
ProxyBench::init(int argc, char** argv)
{
    read_conf(argc, argv);
}

void
ProxyBench::run()
{
    list<ProxyProfile> proxy_profiles;
    list<PingResult> ping_results;

    Conf* conf = Conf::get();

    /*  1. PARSE SUBSCRIPTIONS */

    const auto& parser = SubscrParsers::NewParser(conf->subscr_type);
    for (const auto& addr : conf->subscr_addrs) {
        parser->parse(addr, &proxy_profiles);
    }

    /*  2. RUN PROXIES & PINGING THROUGHT THEM */

    // POSIX semaphore to control concurrency
    sem_t nthreads_avail;
    // mutex to avoid race condition
    std::mutex ping_results_lock;

    sem_init(&nthreads_avail, 0, conf->nthreads);
    // clang-format off
    std::for_each(proxy_profiles.cbegin(), proxy_profiles.cend(),
            [&](const ProxyProfile& p) {
                sem_wait(&nthreads_avail); // -1

                // the new thread created will *wait* for pinging.
                std::thread([&]() {
                    const auto& proxy = Proxys::NewProxy(conf->proxy_type, p);
                    proxy->wait_start();

                    const auto& pinger = Pingers::NewPinger(conf->protocol);
                    const auto& result = pinger->wait_ping(proxy->addr());
                    proxy->stop();

                    ping_results_lock.lock();
                    ping_results.push_back(result);
                    ping_results_lock.unlock();
                });

                sem_post(&nthreads_avail); // +1
            });
    // clang-format on
    sem_destroy(&nthreads_avail);

    /*  3. SORT PING RESULTS */

    std::sort(ping_results.begin(), ping_results.end(),
              [](const PingResult& lhs, const PingResult& rhs) {
                  if (boost::any_cast<int>(lhs.at("delay")) <
                      boost::any_cast<int>(rhs.at("delay"))) {
                      return true;
                  } else {
                      return false;
                  }
              });

    /*  4. FORMAT AND PRINT PING RESULTS */

    PingResultFormatter formatter;
    formatter.init(conf->output_format);

    std::for_each(ping_results.cbegin(), ping_results.cend(),
                  [&formatter](const PingResult& result) {
                      cout << formatter.format(result) << "\n";
                  });
}

void
ProxyBench::exit(enum exit_status e)
{
    switch (e) {
        case exit_status::SUCCESS:
            ::exit(EXIT_SUCCESS);
        default:
            ::exit(EXIT_FAILURE);
    };
}

void
ProxyBench::read_conf(int argc, char* argv[])
{
    try {
        ConfReader reader;
        reader.readConf(argc, argv);
    } catch (const std::exception& e) {
        cout << "Error in CommandLine or ConfigFile\n";
        cout << "  : " << e.what() << "\n";
        this->exit(exit_status::FAILURE);
    }
}
