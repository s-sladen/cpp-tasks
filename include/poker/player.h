#pragma once

#include <string>
#include <stdint.h>

#include "hand.h"

namespace poker {
    class player_t {
    public:
        player_t(std::string name);

        player_t(player_t const&) = delete;
        player_t& operator=(player_t const&) = delete;

        virtual ~player_t();

        std::string const& get_name() const;

        virtual uint32_t bet(uint32_t your_bet, uint32_t max_bet, hand_t const& hand) = 0;

        virtual std::vector<card_t> exchange(hand_t & hand) = 0;

    private:
        std::string name;
    };
}