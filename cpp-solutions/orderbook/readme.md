# Orderbook

## Intro

This homework consists of 2 parts. Note, that the homework is accepted only when these 2 parts are done. However,
you can also get some points for doing part 1 only.

Only part 1 is tested automatically here, the rest is checked manually.

## Problem

In this task you'll implement a small utility for processing trading market data and conduct a few performance experiments.
If you're not familiar with terms like "L2 orderbook", then please read the corresponding [tutorial](https://gitlab.spectral.tech/study/effective-cpp/-/blob/main/tasks/orderbook/intro/readme.md) first.

Namely, you're are given a few samples of market data streams from https://developers.binance.com/docs/derivatives/usds-margined-futures/websocket-market-streams (specifically,
orderbooks). The data was transmitted over network in a specific binary format and then captured and dumped with [Wireshark](https://en.wikipedia.org/wiki/Wireshark) to .pcapng files. These files are then compressed with lz4.

So generally the task consists of the following parts:
- reading and parsing pcapng files (part 1)
- making a few implementations of orderbook (part 2)
- comparing these implementations with `perf_event_open` syscall and writing a small report (also part 2)

## Data format

### Orderbook update format

The original orderbook data is described here:
- https://developers.binance.com/docs/derivatives/usds-margined-futures/websocket-market-streams/Diff-Book-Depth-Streams
- https://developers.binance.com/docs/derivatives/usds-margined-futures/market-data/rest-api/Order-Book
- https://developers.binance.com/docs/derivatives/usds-margined-futures/websocket-market-streams/How-to-manage-a-local-order-book-correctly

However, you have to work with preprocessed data instead.
Note, that each update can have multiple asks/bids updates in a single message, which isn't common for the majority of exchanges. Thats why
data was flattened in this task: every message here contains exactly one update of a bid or ask. Here is the message structure:
```
struct Message {
    uint64_t ts;
    int64_t id;
    uint64_t price;
    uint64_t volume;
    uint8_t conf;
};
```

`ts` is a timestamp (in microseconds), which you can ignore in this task. `id` is an identifier of a message, which you have to validate (described later).

`price` and `volume` is the actual update, serialized from real numbers to integeres. If `d` is the original real valued price, then $price = d \cdot 10^t$, where $t$ is a tick size, which is described below. If `v` is the original real valued volume, then $volume = v \cdot 10^8$.

`conf` is a bitmask, describing the update. Recall, that the original update was flattened. So if the lowest bit of `conf` is 1, then there is a continuation of the update after the current message. If its 0, then the update ends with the current message.

The second lowest bit of `conf` specifies whether the current update is ask (1) or bid (0). The rest of bits are not used.
Note, that there are unused bytes with padding at the end of `Message`, so the total size of it is 40.

The original  data contains both updates and snapshots, which should be treated differently. The data in this task also contains both snapshots and
updates, which is encoded in `ts` field.

If the highest bit in `ts` is 1, it means, that the current message contains meta information:
- the lowest 4 bits of `ts` contain the current value of $t$, described above.
- the next 32 bytes contain the name of the instrument (as zero-terminated string) for the orderbooks in the given file.

There are 2 things guaranteed:
- Every file starts with a meta message.
- Every meta message is followed by a snapshot. Everything else is orderbook updates.

Note, that your utility should process orderbooks only at the end of each original (not flattened) update. The orderbook may be invalid at the middle of processing a series of flattened updates.


### pcapng file format

A series of described `Message` structs was transmitted via UDP protocol (one message = 1 packet) and captured into .pcapng files. Every file contains a series of messages for exactly one instrument.

The file format is described here: https://pcapng.com/. Note, that you don't need to implement a generic parser for these files. Its enough to
be able to parse files for this task. So you can skip the majority of sections in pcap, except for packet blocks, which contain the actual data.

The captured traffic is a regular UDP/IP/Ethernet stack. If you're not quite familiar with network stack, its worth to get a bit acquainted
with TCP/IP stack (https://en.wikipedia.org/wiki/Internet_protocol_suite), since the only difference here is the usage of UDP instead of TCP.

So the parsing of every packet block in pcap should proceed layer by layer as follows:
- Get to `Packet Data` field
  - start parsing Layer 2 Ethernet frame according to https://en.wikipedia.org/wiki/Ethernet_frame (there is no 802.1Q tag)
    - start parsing IPv4 packet according to https://en.wikipedia.org/wiki/Internet_Protocol_version_4#Packet_structure
      - parse UDP packet according to https://en.wikipedia.org/wiki/User_Datagram_Protocol#UDP_datagram_structure
        - parse the payload of UDP packet (`Message` structure)
- Skip the unnecessary fields at the end of each level if there are any. There is enough information in different `length` fields whether there are
some optional fields or not.

Its highly recommended to download wireshark and load given files there to simplify debugging of your parser.

### Download data

You can download all data for this task here: https://drive.google.com/drive/folders/1pvnoU75CMCCZAZayFmtV9YQ2Ntd3W4d-?usp=sharing

The files given to you are compressed with lz4 pcapng files. `result.txt` contains the correct results for each file.

## Task description

### Part 1

Implement function `Parse` in `pcapng.h`, which parses a sequence of messages from a given binary buffer with udp traffic.
This is a slightly simplified interface, since usually its preferable to parse such messages on the fly.

### Part 2

You should make a few implementations of orderbook, each implementation should be able to:
- apply snapshots and updates
- iterate over asks (in increasing order) and bids (in decreasing order)

Maintain only 10000 levels at each side of the orderbook. Lets denote by $a_{20}$ the value of `price * volume` for the 20th ask in the order
and by $b_{20}$ the same value, but for bids. Lets denote $c = a_{20} + b_{20}$. Your utility should calculate the maximum value of $c$ over
all updates of orderbook, i.e. recalculate $c$ after applying each snapshot/update and update the global maximum if needed. Output this global
maximum at the end.

You should implement at least 2 versions of orderbook from this list (and add a test, which checks that results are equal):
1. Using `std::map` with static allocator (since there can only be 20000 nodes in total).
2. Using https://abseil.io/about/design/btree with static allocator.
3. Using price as an index in a circular buffer.

Note, that implementing all 3 of them grants bonus 500 points.

Lets describe the last method with more details. Given that the difference between min and max prices in orderbook isn't that big and prices always change gradually,
its possible to use price as an index in an array. There are a few issues to address here:
- min and max prices change all the time. Thats why this array should be circular. So if a new price is less, than the current minimal one, you can use negative indices. Sometimes its worth to rebuild the orderbook if this array becomes too big or too small.
- This type of indexing gives constant update and erase complexity, however iteration may become inefficient, since you may need to skip some
    empty slots in the array. To counter this allocate another mask array with bits (64-bit words) and mark existing elements there. You can use
`__builtin_ffs` to quickly iterate over set bits and thus over levels of orderbook.

Put your build script into `build.cmake`. It should produce a binary `orderbook`, which should take the following arguments:
```
./orderbook [-t <number>] [files list ...]
```

`-t` specifies the number of threads to use (1 by default), since each of the files can be processed separately. Everything else is files to process.
Note, that each file can be either .pcap or .lz4. In the latter case you should decompress this file first (`liblz4-dev` is installed in the testing system). Note, that your utility
should erase temporary uncompressed files at the end if it creates such.

Output the results as a list
```
filename1:metric1
filename2:metric2
...
```

Where filenames are written as they are given in a command line and metric is formatted with 3 digits after decimal point.

### Perf stats

Add support of the macro `PERF` to your utility. If its set, then:
- use 1 thread
- measure perf-counters of processing a single pcap file. Namely, output the following counters:
  - cache loads and misses (and thus a hit rate) for L1 and LLC caches
  - branches and branch misses
  - instructions and cycles

Use https://man7.org/linux/man-pages/man2/perf_event_open.2.html syscall to measure a part of a program.
Obviously, you shoudn't include decompression into the measurement.

Having this implemented, compare the performance of all orderbook implementations on `btc.pcap` file and write a small report with results.
