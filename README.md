# ProxyBench

[ English | [中文](https://github.com/rtdarwin/ProxyBench#proxybench-1) ]

Benchmark your SS/SSR/V2Ray subscription nodes using ICMP/TCP/HTTP
protocols~

Inspired by
[ShadowsocksBenchmark](https://github.com/Kr328/ShadowsocksBenchmark).

## TODO

- [ ] ICMP/TCP ping
- [ ] Benchmark output format customization

## Features

- Support `SS` `SSR` `V2Ray` proxy
- Support `SS Surge` `SSR Url` subscription
- `HTTP` protocol to benchmark

## Design & Work flow

// TODO

## Build

```bash
cd /path/to/proxy-bench
mkdir build && cd build && cmake .. && make
```

## Usage

ProxyBench will print the following help message when running with
`proxy-bench -h`:

```
Usage: proxy-bench [options] subscr-addr...

Generic Options:
  -h [ --help ]                         show this help
  -V [ --version ]                      print version string
  -c [ --config ] arg                   specify configuration file
  -p [ --proxy ] arg                    ss, ssr, or v2ray
  -e [ --exec ] arg                     path/to/proxy.exec
  -t [ --subscr-type ] arg              ss-surge, ssr-url, or v2ray-vmess-file
  -P [ --ping-protocol ] arg (=icmp http)
                                        icmp, tcp, and http
  -n [ --nping ] arg (=8)               number of pings each protocol
  -S [ --sort-by ] arg (=http)          icmp, tcp, http, or name
  -T [ --num-threads ] arg (=12)        nthreads
  -D [ --export ] arg                   whether and where to export
                                        susbscription nodes
  -F [ --output-format ] arg            // TODO
```

`--proxy-type` `--sbuscr-type` and `sbuscr-addr` options are necessary.

Note that:

1. **option values are case-insensitive**

    ProxyBench accepts `--subscr-type` option present as
    `--subscr-type Ss-suRGe` if you like.

1. **configuration file is supported**

    Write the long command lines into a file and pass it to
    `proxy-bench` by using `--config` options, then you will no longer
    need to type those boring options again.

    For example:

    ```
    proxy = ssr
    exec = /usr/local/bin/ssr-local
    ping-protocol = icmp
    ping-protocol = http
    sort-by = http
    num-threads = 16
    export = /tmp/cc.conf.d
    subscr-type = ssr-url
    subscr-addr = https://www.cc.me/link/your-link-here?mu=0
    ```

    Present `sbuscr-addr` **as a normal options**, just like the above
    example (That's how Boost.Program_Options works).

1. **specify the path of proxy exec if it's not in PATH**

    the execs of proxy are not always in the standard location ($PATH)
    especially when you build the proxy yourself. Use `--exec` option
    to specify the path.

1. **customize the benchmark results that you prefer**

    WIP.

1. **export subscription nodes if you need**

    use `--export' option to specify the directory to export
    subscription nodes.

## Examples

Benchmark SS nodes，sort by http delay:

```
$ proxy-bench  -p ss  -P icmp http  -S http  -t ss-surge  https://www.cc.me/link/your-link-here?is_ss=0
```

Benchmark SSR nodes, all options in config file `cc.me-ssr-url.cfg':

```
$ proxy-bench  -c cc.me-ssr-url.cfg
```

Benchmark V2Ray nodes, sort by TCP delay:

```
$ <Firstly, append all vmess:// protocol links to file ./subscr.file, each with LF(\n) line ending>
$ proxy-bench  -p v2ray  -P tcp http  -S tcp  -t v2ray-vmess-file  ./subscr.file
```

# ProxyBench

[ [English](https://github.com/rtdarwin/ProxyBench#proxybench) | 中文 ]

一个可以用来对你的 SS/SSR/V2Ray 订阅节点跑分的工具，支持 ICMP/TCP/HTTP 三种跑分方式。

Inspired by, 好吧，从
[ShadowsocksBenchmark](https://github.com/Kr328/ShadowsocksBenchmark)
抄了很多代码和设计理念，然后瞎几把改了改（反正大家都是 GPL

## 挖坑

- [ ] ICMP/TCP ping
- [ ] 输出格式可调

## 都抄了哪些功能

- 支持 `SS` `SSR` `V2Ray` 三种代理类型
- 支持 `SS Surge` `SSR Url` 两种订阅类型
- 支持 `HTTP` 跑分

## 我是怎么瞎几把设计的

// TODO

## 编译

```bash
cd /path/to/proxy-bench
mkdir build && cd build && cmake .. && make
```

## 怎么使用这个东西

运行 `proxy-bench -h` 可以看到下面这样的使用帮助：

```
Usage: proxy-bench [options] subscription-addr...

Generic Options:
  -h [ --help ]                         show this help
  -V [ --version ]                      print version string
  -c [ --config ] arg                   specify configuration file
  -p [ --proxy ] arg                    ss, ssr, or v2ray
  -e [ --exec ] arg                     path/to/proxy.exec
  -t [ --subscr-type ] arg              ss-surge, ssr-url, or v2ray-vmess-file
  -P [ --ping-protocol ] arg (=icmp http)
                                        icmp, tcp, and http
  -n [ --nping ] arg (=8)               number of pings each protocol
  -S [ --sort-by ] arg (=http)          icmp, tcp, http, or name
  -T [ --num-threads ] arg (=12)        nthreads
  -D [ --export ] arg                   whether and where to export
                                        susbscription nodes
  -F [ --output-format ] arg            // TODO
```

其中 `--proxy-type` `--subscr-type` 选项和最后的订阅地址
`subscription-addr` 是必需的参数。

当然了：

1. **防手抖参数值不区分大小写**

    使用帮助中给出的参数值示例是小写的，如果你喜欢大写，或者手抖打成了
    这样 `--subscr-type Ss-suRGe` 也是可以的，选项值不区分大小写。

1. **可将命令行参数写入文件中**

    每次都在命令行上打一串参数是很无聊的，你可以把这些参数都写在配置文
    件中，然后用 `--config` 参数告诉 `proxy-bench` 去读取这个配置文件。
    比如：

    ```
    proxy = ssr
    exec = /usr/local/bin/ssr-local
    ping-protocol = icmp
    ping-protocol = http
    sort-by = http
    num-threads = 16
    export = /tmp/cc.conf.d
    subscr-type = ssr-url
    subscr-addr = https://www.cc.me/link/your-link-here?mu=0
    ```

    唯一要注意的一点就是订阅地址要使用 `subscr-addr` 来指定，其他与命
    令行长参数一致。

    `/examples' 目录中放置了一些可以用来更改的模板。

1. **可自己指定代理可执行文件的位置**

    如果你的代理程序不在 `$PATH` 中，可以使用 `--exec` 选项指定。比如
    `--exec my/own/path/v2ray`。

1. **输出格式可调**

    Hmmm，先挖个坑。

1. **可导出订阅节点**

    用 `--export' 参数来指定导出目录。

## 使用示例

SS 节点跑分，以 HTTP 延迟为基准排序：

```
$ proxy-bench  -p ss  -P icmp http  -S http  -t ss-surge  https://www.cc.me/link/your-link-here?is_ss=0
```


SSR 节点跑分，使用配置文件 cc.me-ssr-url.cfg

```
$ proxy-bench  -c cc.me-ssr-url.cfg
```

V2Ray 节点跑分，以 TCP 延迟为基准排序：

```
$ <先把所有 vmess:// 链接放入文件 ./subscr.file 中，每个链接用 LF(\n) 结尾>
$ proxy-bench  -p v2ray  -P tcp http  -S tcp  -t v2ray-vmess-file  ./subscr.file
```
