#pragma once

#include "card.h"

#include <vector>

namespace poker {
    class hand_t {
    public:
        hand_t(uint32_t cash);

        hand_t(hand_t && hand);

        hand_t(hand_t const&) = delete;

        hand_t& operator=(hand_t hand);

        std::vector<card_t> const& get_cards() const;

        card_t take_card(size_t index);

        void add_card(card_t card);

        bool has_cards() const;

        uint32_t get_cash() const;

        uint32_t take_cash(uint32_t cash);

        uint32_t put_cash(uint32_t cash);
    private:
        std::vector<card_t> cards;
        uint32_t cash;
    };
}