set(ProxyBench_SRCS
  main.cc

  ConfReader.cc
  PingResultFormatter.cc
  ProxyBench.cc
  SyncDnsResolver.cc

  ping/Pinging.cc
  ping/Http_Pinger.cc
  ping/Icmp_Pinger.cc
  ping/Tcp_Pinger.cc

  proxy/Proxy.cc
  proxy/SS_Proxy.cc
  proxy/SSR_Proxy.cc
  proxy/V2Ray_Proxy.cc

  subscr-parser/SubscrParser.cc
  subscr-parser/SSSurge_SubscrParser.cc
  subscr-parser/SSRUrl_SubscrParser.cc
  subscr-parser/V2RayVmessFile_SubscrParser.cc
  )

add_executable(proxy-bench "${ProxyBench_SRCS}")

target_link_libraries(proxy-bench
  Boost::program_options
  )
