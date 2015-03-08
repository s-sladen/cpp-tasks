#pragma once

#include <stdint.h>

#include "hand.h"

namespace poker {
    enum combination : uint32_t {
        STRAIGHT_FLUSH  = 0x09000000,
        QUADS           = 0x08000000,
        FULL_HOUSE      = 0x07000000,
        FLUSH           = 0x06000000,
        STRAIGHT        = 0x05000000,
        SET             = 0x04000000,
        TWO_PAIRS       = 0x03000000,
        PAIR            = 0x02000000,
        HIGHEST_CARD    = 0x01000000
    };


    struct simple_rules {
        static uint32_t get_combination_rank(hand_t const& hand);

        static const uint32_t SMALL_BLIND;

        static const uint32_t BIG_BLIND;
    };

    std::string get_combination_name(uint32_t combination_rank);
}