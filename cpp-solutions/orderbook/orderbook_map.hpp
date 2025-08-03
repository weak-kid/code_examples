#pragma once

#include "types.hpp"
#include <map>
#include <memory>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <algorithm>
#include <cstddef>
#include <new>
#include <type_traits>

template <typename T, std::size_t MaxElements>
class StaticAllocator {
public:
    using value_type        = T;
    using pointer           = T*;
    using const_pointer     = const T*;
    using reference         = T&;
    using const_reference   = const T&;
    using size_type         = std::size_t;
    using difference_type   = std::ptrdiff_t;

    template <typename U>
    struct rebind {
        using other = StaticAllocator<U, MaxElements>;
    };

    StaticAllocator() noexcept = default;
    StaticAllocator(const StaticAllocator&) noexcept = default;
    template <typename U>
    StaticAllocator(const StaticAllocator<U, MaxElements>&) noexcept {}
    ~StaticAllocator() = default;

    pointer allocate(size_type n) {
        if (n != 1) {
            std::terminate();
        }
        if (free_count_ > 0) {
            std::size_t idx = free_list_[--free_count_];
            return reinterpret_cast<pointer>(buffer_ + idx * sizeof(T));
        }
        if (offset_ < MaxElements) {
            std::size_t idx = offset_++;
            return reinterpret_cast<pointer>(buffer_ + idx * sizeof(T));
        }
        std::terminate();
    }

    void deallocate(pointer p, size_type n) noexcept {
        if (p == nullptr || n != 1) return;
        auto byte_ptr = reinterpret_cast<unsigned char*>(p);
        std::size_t idx = (byte_ptr - buffer_) / sizeof(T);
        if (idx < MaxElements) {
            free_list_[free_count_++] = idx;
        }
    }

    size_type max_size() const noexcept {
        return MaxElements;
    }

    template <typename U>
    bool operator==(const StaticAllocator<U, MaxElements>&) const noexcept {
        return true;
    }
    template <typename U>
    bool operator!=(const StaticAllocator<U, MaxElements>&) const noexcept {
        return false;
    }

    static void reset_pool() noexcept {
        offset_     = 0;
        free_count_ = 0;
    }

private:
    inline static std::size_t offset_ = 0;
    inline static std::size_t free_list_[MaxElements] = {};
    inline static std::size_t free_count_ = 0;
    inline static unsigned char buffer_[MaxElements * sizeof(T)] alignas(alignof(T));
};



class MapOrderBook : public OrderBook {
    using Key = uint64_t;
    using Value = uint64_t;
    std::map<Key, Value, std::less<Key>, 
                           StaticAllocator<std::pair<const Key, Value>, 20000>> asks_;
    std::map<Key, Value, std::greater<Key>, 
                           StaticAllocator<std::pair<const Key, Value>, 20000>> bids_;
    uint8_t tick_size_ = 0;

public:
    void apply_snapshot(const std::vector<std::pair<uint64_t, uint64_t>>& asks,
                      const std::vector<std::pair<uint64_t, uint64_t>>& bids) override {
        asks_.clear();
        bids_.clear();
        auto sorted_asks = asks;
        std::sort(sorted_asks.begin(), sorted_asks.end(),
                  [](auto const &a, auto const &b) { return a.first < b.first; });
        if (sorted_asks.size() > 10000) {
            sorted_asks.resize(10000);
        }

        auto sorted_bids = bids;
        std::sort(sorted_bids.begin(), sorted_bids.end(),
                  [](auto const &a, auto const &b) { return a.first > b.first; });
        if (sorted_bids.size() > 10000) {
            sorted_bids.resize(10000);
        }
        for (const auto& [price, volume] : sorted_asks) {
            if (volume > 0) asks_.emplace(price, volume);
        }

        for (const auto& [price, volume] : sorted_bids) {
            if (volume > 0) bids_.emplace(price, volume);
        }
    }

    void apply_update(uint64_t price, uint64_t volume, bool is_ask) override {
        if (is_ask) {
            update(asks_, price, volume, false);
        } else {
            update(bids_, price, volume, true);
        }
    }

    double get_a20() const override {
        return get_level(asks_, 20);
    }

    double get_b20() const override {
        return get_level(bids_, 20);
    }

    void set_tick_size(uint8_t tick_size) override { tick_size_ = tick_size; }

private:
    template <typename Map>
    void update(Map& side, uint64_t price, uint64_t volume, bool higher) {
        if (side.size() < 10000 || volume == 0|| side.find(price) != side.end()) {       
            if (volume == 0) {
                side.erase(price);
            } else {
                side[price] = volume;
            }
            return;
        }
        uint64_t worst_price = std::prev(side.end())->first;
        if (higher && price <= worst_price || !higher && price >= worst_price) {
            return;
        }
        side.erase(worst_price);
        side.emplace(price, volume);
    }


    template <typename Map>
    double get_level(const Map& side, size_t level) const {
        if (side.size() < level) return 0.0;

        auto it = side.begin();
        std::advance(it, level - 1);
        const double price = static_cast<double>(it->first) / std::pow(10, tick_size_);
        const double volume = static_cast<double>(it->second) / 1e8;
        return price * volume;
    }
};
