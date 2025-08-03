#pragma once

#include <vector>
#include <cstdint>

class OrderBook {
public:
    virtual ~OrderBook() = default;
    
    virtual void apply_snapshot(
        const std::vector<std::pair<uint64_t, uint64_t>>& asks,
        const std::vector<std::pair<uint64_t, uint64_t>>& bids) = 0;
    
    virtual void apply_update(
        uint64_t price,
        uint64_t volume,
        bool is_ask) = 0;
    
    virtual double get_a20() const = 0;
    virtual double get_b20() const = 0;

    virtual void set_tick_size(uint8_t tick_size) = 0;
};

struct Message {
    uint64_t ts;
    int64_t id;
    uint64_t price;
    uint64_t volume;
    uint8_t conf;
};
