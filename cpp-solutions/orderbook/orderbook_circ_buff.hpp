#pragma once

#include "types.hpp"
#include <vector>
#include <cstdint>
#include <cassert>
#include <cmath>
#include <algorithm>
#include <limits>

class CircularSide {
public:
    CircularSide()
        : base_price_(0),
          capacity_(0),
          size_(0)
    {}

    void snapshot(const std::vector<std::pair<uint64_t, uint64_t>>& levels) {
        if (levels.empty()) {
            clear_all();
            return;
        }
        uint64_t min_price = std::numeric_limits<uint64_t>::max();
        uint64_t max_price = 0;
        for (auto const& p : levels) {
            min_price = std::min(min_price, p.first);
            max_price = std::max(max_price, p.first);
        }
        uint64_t needed_range = max_price - min_price + 1;
        uint64_t new_cap = 1;
        while (new_cap < needed_range) new_cap <<= 1;
        base_price_ = min_price;
        capacity_   = static_cast<size_t>(new_cap);
        size_       = 0;
        volumes_.assign(capacity_, 0);
        mask_.assign((capacity_ + 63) / 64, 0ULL);
        size_t inserted = 0;
        for (auto const& p : levels) {
            uint64_t price = p.first;
            uint64_t vol   = p.second;
            size_t idx = static_cast<size_t>(price - base_price_);
            assert(idx < capacity_);
            if (vol > 0) {
                volumes_[idx] = vol;
                set_mask_bit(idx);
                ++inserted;
            }
        }
        size_ = inserted;
    }

    void update(uint64_t price, uint64_t volume) {
        if (!in_range(price)) {
            resize_to_include(price);
        }
        size_t idx = static_cast<size_t>(price - base_price_);
        assert(idx < capacity_);
        if (volume == 0) {
            if (get_mask_bit(idx)) {
                volumes_[idx] = 0;
                clear_mask_bit(idx);
                --size_;
            }
        } else {
            if (!get_mask_bit(idx)) {
                volumes_[idx] = volume;
                set_mask_bit(idx);
                ++size_;
            } else {
                volumes_[idx] = volume;
            }
        }
    }

    double get_nth(bool highest, size_t N) const {
        if (N == 0 || size_ < N) return 0.0;
        size_t found = 0;
        double result = 0.0;
        if (!highest) {
            size_t num_words = mask_.size();
            for (size_t w = 0; w < num_words; ++w) {
                uint64_t word = mask_[w];
                while (word != 0ULL) {
                    int bitpos = __builtin_ffsll(word) - 1;
                    size_t idx = (w << 6) + static_cast<size_t>(bitpos);
                    if (idx >= capacity_) break;
                    ++found;
                    if (found == N) {
                        uint64_t price = base_price_ + idx;
                        uint64_t vol   = volumes_[idx];
                        return (static_cast<double>(price) / std::pow(10, tick_size_)) * (static_cast<double>(vol) / 1e8);
                    }
                    word &= (word - 1ULL);
                }
            }
        } else {
            size_t num_words = mask_.size();
            for (size_t w = num_words; w-- > 0; ) {
                uint64_t word = mask_[w];
                while (word != 0ULL) {
                    int lz = __builtin_clzll(word);
                    int bitpos = 63 - lz;
                    size_t idx = (w << 6) + static_cast<size_t>(bitpos);
                    if (idx >= capacity_) {
                        word &= (word - 1ULL);
                        continue;
                    }
                    ++found;
                    if (found == N) {
                        uint64_t price = base_price_ + idx;
                        uint64_t vol   = volumes_[idx];
                        return (static_cast<double>(price) / std::pow(10, tick_size_)) * (static_cast<double>(vol) / 1e8);
                    }
                    word &= ~(1ULL << bitpos);
                }
            }
        }
        return 0.0;
    }

    void set_tick_size(uint8_t tick_size) {
        tick_size_ = tick_size;
    }

size_t size() const {
        return size_;
    }

    bool has_price(uint64_t price) const {
        if (!in_range(price)) return false;
        size_t idx = static_cast<size_t>(price - base_price_);
        return get_mask_bit(idx);
    }

    uint64_t get_min_price() const {
        if (size_ == 0) return 0;
        size_t num_words = mask_.size();
        for (size_t w = 0; w < num_words; ++w) {
            uint64_t word = mask_[w];
            if (word == 0ULL) continue;
            int bitpos = __builtin_ffsll(word) - 1;
            size_t idx = (w << 6) + static_cast<size_t>(bitpos);
            if (idx >= capacity_) continue;
            return base_price_ + idx;
        }
        return 0;
    }

    uint64_t get_max_price() const {
        if (size_ == 0) return 0;
        size_t num_words = mask_.size();
        for (size_t w = num_words; w-- > 0; ) {
            uint64_t word = mask_[w];
            if (word == 0ULL) continue;
            int lz = __builtin_clzll(word);
            int bitpos = 63 - lz;
            size_t idx = (w << 6) + static_cast<size_t>(bitpos);
            if (idx >= capacity_) continue;
            return base_price_ + idx;
        }
        return 0;
    }

    size_t get_min_index() const {
        size_t num_words = mask_.size();
        for (size_t w = 0; w < num_words; ++w) {
            uint64_t word = mask_[w];
            if (word == 0ULL) continue;
            int bitpos = __builtin_ffsll(word) - 1;
            size_t idx = (w << 6) + static_cast<size_t>(bitpos);
            if (idx < capacity_) return idx;
        }
        return capacity_;
    }

    size_t get_max_index() const {
        size_t num_words = mask_.size();
        for (size_t w = num_words; w-- > 0; ) {
            uint64_t word = mask_[w];
            if (word == 0ULL) continue;
            int lz = __builtin_clzll(word);
            int bitpos = 63 - lz;
            size_t idx = (w << 6) + static_cast<size_t>(bitpos);
            if (idx < capacity_) return idx;
        }
        return capacity_;
    }

    void remove_index(size_t idx) {
        if (idx >= capacity_) return;
        if (get_mask_bit(idx)) {
            volumes_[idx] = 0;
            clear_mask_bit(idx);
            --size_;
        }
    }

private:
    uint8_t tick_size_;
    uint64_t base_price_;
    size_t   capacity_;
    size_t   size_;
    std::vector<uint64_t> volumes_;
    std::vector<uint64_t> mask_;

    void clear_all() {
        base_price_ = 0;
        capacity_   = 0;
        size_       = 0;
        volumes_.clear();
        mask_.clear();
    }

    bool in_range(uint64_t price) const {
        if (capacity_ == 0) return false;
        uint64_t max_price = base_price_ + static_cast<uint64_t>(capacity_ - 1);
        return (price >= base_price_ && price <= max_price);
    }

    void resize_to_include(uint64_t price) {
        if (capacity_ == 0) {
            base_price_ = price;
            capacity_   = 1;
            while (capacity_ < 1) capacity_ <<= 1;
            volumes_.assign(capacity_, 0);
            mask_.assign((capacity_ + 63) / 64, 0ULL);
            return;
        }
        uint64_t old_min = base_price_;
        uint64_t old_max = base_price_ + static_cast<uint64_t>(capacity_ - 1);
        uint64_t new_min = std::min(old_min, price);
        uint64_t new_max = std::max(old_max, price);
        uint64_t needed_range = new_max - new_min + 1;
        uint64_t new_cap = 1;
        while (new_cap < needed_range) new_cap <<= 1;
        if (new_cap == capacity_) {
            uint64_t shift = old_min - new_min;
            std::vector<uint64_t> new_vol(capacity_, 0);
            std::vector<uint64_t> new_m((capacity_ + 63) / 64, 0ULL);
            for (size_t i = 0; i < capacity_; ++i) {
                if (get_mask_bit(i)) {
                    uint64_t v = volumes_[i];
                    size_t ni = static_cast<size_t>(i + shift);
                    new_vol[ni] = v;
                    size_t w = ni >> 6;
                    size_t b = ni & 63;
                    new_m[w] |= (1ULL << b);
                }
            }
            volumes_.swap(new_vol);
            mask_.swap(new_m);
            base_price_ = new_min;
        } else {
            size_t old_cap = capacity_;
            uint64_t old_base = base_price_;
            std::vector<uint64_t> new_vol(static_cast<size_t>(new_cap), 0ULL);
            std::vector<uint64_t> new_m((static_cast<size_t>(new_cap) + 63) / 64, 0ULL);
            uint64_t shift = old_base - new_min;
            for (size_t i = 0; i < old_cap; ++i) {
                if (get_mask_bit(i)) {
                    uint64_t v = volumes_[i];
                    size_t ni = static_cast<size_t>(i + shift);
                    new_vol[ni] = v;
                    size_t w = ni >> 6;
                    size_t b = ni & 63;
                    new_m[w] |= (1ULL << b);
                }
            }
            volumes_.swap(new_vol);
            mask_.swap(new_m);
            base_price_ = new_min;
            capacity_   = static_cast<size_t>(new_cap);
        }
    }

    inline void set_mask_bit(size_t i) {
        size_t w = i >> 6;
        size_t b = i & 63;
        mask_[w] |= (1ULL << b);
    }

    inline void clear_mask_bit(size_t i) {
        size_t w = i >> 6;
        size_t b = i & 63;
        mask_[w] &= ~(1ULL << b);
    }

    inline bool get_mask_bit(size_t i) const {
        size_t w = i >> 6;
        size_t b = i & 63;
        return ((mask_[w] >> b) & 1ULL) != 0ULL;
    }
};

class CircularOrderBook : public OrderBook {
public:
    CircularOrderBook() {}

    ~CircularOrderBook() override = default;

    
    void apply_snapshot(
        const std::vector<std::pair<uint64_t, uint64_t>>& asks,
        const std::vector<std::pair<uint64_t, uint64_t>>& bids
    ) override {
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

        ask_side_.snapshot(sorted_asks);
        bid_side_.snapshot(sorted_bids);
    }

    void apply_update(
        uint64_t price,
        uint64_t volume,
        bool is_ask
    ) override {
        if (is_ask) {
            if (volume == 0 || ask_side_.has_price(price) || ask_side_.size() < 10000) {
                ask_side_.update(price, volume);
                return;
            }
            uint64_t worst_price = ask_side_.get_max_price();
            if (price >= worst_price) {
                return;
            }
            ask_side_.update(price, volume);
            size_t remove_idx = ask_side_.get_max_index();
            ask_side_.remove_index(remove_idx);
        } else {
            if (volume == 0 || bid_side_.has_price(price) || bid_side_.size() < 10000) {
                bid_side_.update(price, volume);
                return;
            }
            uint64_t worst_price = bid_side_.get_min_price();
            if (price <= worst_price) {
                return;
            }
            bid_side_.update(price, volume);
            size_t remove_idx = bid_side_.get_min_index();
            bid_side_.remove_index(remove_idx);
        }
    }

    double get_a20() const override {
        return ask_side_.get_nth(false, 20);
    }

    double get_b20() const override {
        return bid_side_.get_nth(true, 20);
    }

    void set_tick_size(uint8_t tick_size) override {
        ask_side_.set_tick_size(tick_size);
        bid_side_.set_tick_size(tick_size);
    }

private:
    CircularSide ask_side_;
    CircularSide bid_side_;
};
