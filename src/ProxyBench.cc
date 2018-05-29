#include "ProxyBench.hh"

#include <semaphore.h>

#include <algorithm>
#include <atomic>
#include <boost/any.hpp>
#include <exception>
#include <iostream>
#include <list>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "Conf.hh"
#include "ConfReader.hh"
#include "PingResultFormatter.hh"
#include "Pinging.hh"
#include "Proxy.hh"
#include "SubscrParser.hh"
#include "ev.h"

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

    /*  0. EVENTLOOP FIRST (for timer) */

    std::thread evloop_thread([]() {
        struct ev_loop* loop = EV_DEFAULT;
        ev_run(loop);
    });
    evloop_thread.detach();

    /*  1. PARSE SUBSCRIPTIONS */

    const auto& parser = SubscrParsers::NewParser(conf->subscr_type);
    for (const auto& addr : conf->subscr_addrs) {
        parser->parse(addr, &proxy_profiles);
    }

    /*  2. PINGING THROUGHT PROXIES */

    // POSIX semaphore to control concurrency
    sem_t nthreads_avail;
    // mutex to avoid race condition
    std::mutex ping_results_lock;
    std::atomic_int lport(23333);

    sem_init(&nthreads_avail, 0, conf->nthreads);
    for (auto& profile : proxy_profiles) {
        sem_wait(&nthreads_avail); // -1

        std::thread new_ping_thread([&]() {
            int curr_port = lport++;
            profile["local_port"] = std::to_string(curr_port);

            vector<std::shared_ptr<Pinger>> pingers;
            {
                for (const auto& proto : conf->protocols) {
                    pingers.push_back(Pingers::NewPinger(proto));
                }
            }

            std::shared_ptr<Proxy> proxy;
            int require_proxy = 0;
            {
                proxy = Proxys::NewProxy(conf->proxy_type, profile);
                if (!conf->export_dir.empty()) {
                    proxy->dump_config_file(conf->export_dir);
                }

                for (const auto& p : pingers) {
                    require_proxy += p->require_proxy();
                };
                if (require_proxy) {
                    proxy->wait_start();
                }
            }

            PingResult result;
            {
                for (const auto& pinger : pingers) {
                    pinger
                        ->socks5_proxy("socks5h://127.0.0.1:" +
                                       std::to_string(curr_port))
                        .wait_ping(&result);
                }
                result["profile"] = profile;

                ping_results_lock.lock();
                ping_results.push_back(std::move(result));
                ping_results_lock.unlock();
            }

            if (proxy)
                proxy->stop();

            sem_post(&nthreads_avail); // +1
        });

        new_ping_thread.detach();
    }

    // wait for all ping threads
    for (int i = 0; i < conf->nthreads; i++) {
        sem_wait(&nthreads_avail);
    }
    sem_destroy(&nthreads_avail);

    /*  3. SORT PING RESULTS */

    std::string sortby = conf->sort_by;
    ping_results.sort([&sortby](const PingResult& lhs,
                                const PingResult& rhs) -> bool {
        if (sortby == "name") {
            return strcmp(boost::any_cast<string>(
                              boost::any_cast<ProxyProfile>(lhs.at("profile"))
                                  .at("name"))
                              .c_str(),
                          boost::any_cast<string>(
                              boost::any_cast<ProxyProfile>(rhs.at("profile"))
                                  .at("name"))
                              .c_str()) < 0;
        } else {
            return (unsigned)boost::any_cast<int>(lhs.at(sortby + "_delay")) <
                   (unsigned)boost::any_cast<int>(rhs.at(sortby + "_delay"));
        }
    });

    /*  4. FORMAT AND PRINT PING RESULTS */

    PingResultFormatter formatter;
    formatter.init(conf->output_format);

    for (const auto& result : ping_results) {
        cout << formatter.format(result) << "\n";
    }
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
