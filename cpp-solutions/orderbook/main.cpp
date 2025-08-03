#include "types.hpp"
#include "pcapng.hpp"
#include "orderbook_circ_buff.hpp"
#include "orderbook_map.hpp"
#include "pref.hpp"
#include <thread>
#include <mutex>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <iomanip>
#include <lz4frame.h>
#include <vector>

std::vector<uint8_t> decompress_lz4(std::vector<uint8_t> src) {
    size_t src_size = src.size();
    LZ4F_decompressionContext_t ctx;
    LZ4F_createDecompressionContext(&ctx, LZ4F_VERSION);

    LZ4F_frameInfo_t frame_info;
    size_t header_size = src_size;
    LZ4F_getFrameInfo(ctx, &frame_info, src.data(), &header_size);

    std::vector<uint8_t> output;
    if (frame_info.contentSize > 0) {
        output.reserve(frame_info.contentSize);
    }

    const uint8_t* src_ptr = src.data() + header_size;
    size_t src_remaining = src_size - header_size;
    char buffer[64 * 1024];

    while (src_remaining > 0) {
        size_t dst_size = sizeof(buffer);
        size_t src_consumed = src_remaining;

        size_t res = LZ4F_decompress(
            ctx,
            buffer, &dst_size,
            src_ptr, &src_consumed,
            nullptr
        );

        output.insert(output.end(), buffer, buffer + dst_size);

        src_ptr += src_consumed;
        src_remaining -= src_consumed;

        if (res == 0) break;
    }

    LZ4F_freeDecompressionContext(ctx);
    return output;
}


void process_file(const std::string& filename, double& result) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }
    size_t size = file.tellg();
    file.seekg(0);
    std::vector<uint8_t> buffer(size);
    file.read(reinterpret_cast<char*>(buffer.data()), size);
    if (filename.substr(filename.find_last_of(".") + 1) == "lz4") {
        buffer = decompress_lz4(buffer);
    }
    std::vector<Message> messages = Parse(std::string_view(reinterpret_cast<char*>(buffer.data()), buffer.size()));

    #ifdef PERF
    auto perf = setup_perf();
    perf.start();
    #endif
    
    std::unique_ptr<OrderBook> book;
    double current_max = 0.0;
    bool in_update_sequence = false;
    bool is_snapshot = false;

    for (size_t i = 0; i < messages.size(); ++i) {
        if (messages[i].ts & (1ULL << 63)) {
            #ifndef USEMAP
            book = std::make_unique<CircularOrderBook>();
            #endif
            #ifdef USEMAP
            book = std::make_unique<MapOrderBook>();
            #endif
            is_snapshot = true;
            book->set_tick_size(messages[i].ts & 0xF);
            continue;
        }
        if (is_snapshot) {
            std::vector<std::pair<uint64_t, uint64_t>> asks_, bids_;
            while (is_snapshot) {
                if (messages[i].conf & 0x2) {
                    asks_.emplace_back(messages[i].price, messages[i].volume);
                } else {
                    bids_.emplace_back(messages[i].price, messages[i].volume);
                }
                is_snapshot = messages[i].conf & 0x1;
                if (is_snapshot) {
                    ++i;
                }
            }
            book->apply_snapshot(asks_, bids_);
        } else {
            if (!in_update_sequence) {
                const double c = book->get_a20() + book->get_b20();
                current_max = std::max(current_max, c);
            }
            book->apply_update(messages[i].price, messages[i].volume, messages[i].conf & 0x2);
            in_update_sequence = messages[i].conf & 0x1;
        }
    }
    const double c = book->get_a20() + book->get_b20();
    current_max = std::max(current_max, c);

    result = current_max;

    #ifdef PERF
    perf.stop();
    perf.print();
    #endif
}

std::mutex result_mutex;
std::unordered_map<std::string, double> results;

void process_file_thread(const std::string& filename) {
    double res;
    process_file(filename, res);
    
    std::lock_guard<std::mutex> lock(result_mutex);
    results[filename] = res;
}

int main(int argc, char** argv) {
    size_t max_threads = 1;
    if (argc < 2 || (argv[1] == "-t" && argc < 4)) {
        std::cerr << "No filename\n";
    }
    size_t ind = 1;
    if (strcmp(argv[1], "-t") == 0) {
        ind = 3;
        #ifndef PERF
        max_threads = atoi(argv[2]);
        #endif
    }
    size_t files_count = argc - ind;
    std::vector<std::string> files(files_count);
    max_threads = std::min(max_threads, files_count);
    for (size_t i = 0; ind + i < argc; ++i) {
        files[i] = argv[ind + i];
    }
    std::vector<std::thread> workers;
    for (const auto& file : files) {
        workers.emplace_back(process_file_thread, file);
        if (workers.size() >= max_threads) {
            for (auto& t : workers) t.join();
            workers.clear();
        }
    }
    
    for (const auto& [file, metric] : results) {
        std::cout << file << ":" << std::fixed << std::setprecision(3) << metric << "\n";
    }
}