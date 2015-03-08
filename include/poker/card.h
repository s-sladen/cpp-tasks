#pragma once

#include <string>

namespace poker {
    enum card_suit : uint32_t {
        CLUBS,
        DIAMONDS,
        HEARTS,
        SPADES
    };

    enum card_rank : uint32_t {
        D2,
        D3,
        D4,
        D5,
        D6,
        D7,
        D8,
        D9,
        D10,
        JACK,
        QUEEN,
        KING,
        ACE
    };

    class card_t {
    public:
        card_t(card_t && orig);

        card_t(card_t const&) = delete;

        ~card_t();

        card_t& operator=(card_t card);

        card_suit get_suit() const;

        card_rank get_rank() const;

        operator std::string() const;

    private:
        card_t(card_suit suit, card_rank rank);

        card_suit suit;
        card_rank rank;

        friend class deck_t;
    };

    std::string suit_name(card_suit suit);

    std::string rank_name(card_rank rank);

    std::ostream& operator<<(std::ostream& os, card_t const& card);
}