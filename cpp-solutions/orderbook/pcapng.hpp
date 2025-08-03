#pragma once

#include <cstdint>
#include <functional>
#include <cstring>
#include <string_view>
#include "types.hpp"

inline uint16_t get_uint16_be(const uint8_t *ptr) {
    return (ptr[0] << 8) | ptr[1];
}

inline uint64_t get_uint64(const uint8_t *ptr) {
    return ((uint64_t)ptr[7] << 56) | ((uint64_t)ptr[6] << 48) | ((uint64_t)ptr[5] << 40) | ((uint64_t)ptr[4] << 32) |
           ((uint64_t)ptr[3] << 24) | ((uint64_t)ptr[2] << 16) | ((uint64_t)ptr[1] << 8) | (uint64_t)ptr[0];
}

inline int64_t get_int64(const uint8_t *ptr) {
    return static_cast<int64_t>(get_uint64(ptr));
}

inline uint32_t get_uint32(const uint8_t *ptr) {
    return ptr[0] | (ptr[1] << 8) | (ptr[2] << 16) | (ptr[3] << 24);
}

inline std::vector<Message> Parse(std::string_view data) {
    const uint8_t *current_ptr = reinterpret_cast<const uint8_t*>(data.data());
    std::vector<Message> result;
    size_t size = data.size();
    const uint8_t *end_ptr = current_ptr + size;

    while (current_ptr + 8 <= end_ptr) {
        uint32_t block_type = get_uint32(current_ptr);
        uint32_t block_total_length = get_uint32(current_ptr + 4);

        if (block_total_length < 8 || current_ptr + block_total_length > end_ptr) {
            break;
        }

        if (block_type == 0x00000006) {
            const uint8_t *block_start = current_ptr;

            uint32_t captured_packet_length = get_uint32(block_start + 20);
            const uint8_t *packet_data_ptr = block_start + 28;

            if (captured_packet_length < 14) {
                current_ptr += block_total_length;
                continue;
            }

            const uint8_t *ether_header = packet_data_ptr;
            uint16_t ether_type = get_uint16_be(ether_header + 12);
            if (ether_type != 0x0800) {
                current_ptr += block_total_length;
                continue;
            }

            const uint8_t *ip_header = ether_header + 14;
            uint8_t version_ihl = ip_header[0];
            uint8_t ihl = version_ihl & 0x0F;
            uint16_t ip_header_length = ihl * 4;
            if (ip_header_length < 20 || captured_packet_length < 14 + ip_header_length + 8 + 40) {
                current_ptr += block_total_length;
                continue;
            }

            if (ip_header[9] != 17) {
                current_ptr += block_total_length;
                continue;
            }

            const uint8_t *udp_header = ip_header + ip_header_length;
            uint16_t udp_length = get_uint16_be(udp_header + 4);
            if (udp_length != 48) {
                current_ptr += block_total_length;
                continue;
            }

            const uint8_t *message_ptr = udp_header + 8;

            Message msg;
            msg.ts = get_uint64(message_ptr);
            msg.id = get_int64(message_ptr + 8);
            msg.price = get_uint64(message_ptr + 16);
            msg.volume = get_uint64(message_ptr + 24);
            msg.conf = message_ptr[32];

            result.push_back(msg);
        }

        current_ptr += block_total_length;
    }
    return result;
}
