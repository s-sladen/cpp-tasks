#include <poker/deck.h>

namespace poker {
    deck_t::deck_t() {
        for (size_t suit = card_suit::CLUBS; suit <= card_suit::SPADES; ++suit)
            for (size_t rank = card_rank::D2; rank <= card_rank::ACE; ++rank)
                cards.push_back(card_t(static_cast<card_suit>(suit), static_cast<card_rank>(rank)));
    }

    card_t deck_t::take() {
        card_t result = std::move(cards.front());
        cards.pop_front();
        return result;
    }

    void deck_t::put(card_t card) {
        cards.push_back(std::move(card));
    }

    void deck_t::reshuffle() {
        std::random_shuffle(cards.begin(), cards.end());
    }
}