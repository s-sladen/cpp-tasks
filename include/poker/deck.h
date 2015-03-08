#pragma once

#include <deque>

#include "card.h"

namespace poker {
    class deck_t {
    public:
        deck_t();

        deck_t(deck_t const&) = delete;
        deck_t& operator=(deck_t const&) = delete;

        card_t take();

        void put(card_t card);

        void reshuffle();
    private:
        std::deque<card_t> cards;
    };
}