#pragma once

#include "player.h"

namespace poker {
    class human_player_t : public player_t {
    public:
        human_player_t(std::string name);

        ~human_player_t();

        uint32_t bet(uint32_t your_bet, uint32_t max_bet, hand_t const& hand);

        std::vector<card_t> exchange(hand_t & hand);
    private:
        void print_intro(hand_t const& hand) const;
    };
}