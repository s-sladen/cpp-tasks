#include <poker/card.h>

#include <cstdlib>

namespace poker {
    card_t::card_t(card_suit suit, card_rank rank)
            : suit(suit)
            , rank(rank)
    {}

    card_t::card_t(card_t&& card)
            : suit(card.suit)
            , rank(card.rank)
    {}

    card_t::~card_t() {
    }

    card_t& card_t::operator=(card_t card) {
        std::swap(suit, card.suit);
        std::swap(rank, card.rank);
        return *this;
    }

    card_rank card_t::get_rank() const {
        return rank;
    }

    card_suit card_t::get_suit() const {
        return suit;
    }

    std::string suit_name(card_suit suit) {
        switch (suit) {
            case card_suit::CLUBS:
                return "♣";
            case card_suit::DIAMONDS:
                return "♦";
            case card_suit::HEARTS:
                return "♥";
            case card_suit::SPADES:
                return "♠";
        }
    }

    std::string rank_name(card_rank rank) {
        switch (rank) {
            case card_rank::ACE:
                return "A";
            case card_rank::KING:
                return "K";
            case card_rank::QUEEN:
                return "Q";
            case card_rank::JACK:
                return "J";
            default:
                return std::to_string(static_cast<uint32_t>(rank + 2 - card_rank::D2));
        }
    }

    card_t::operator std::string() const {
        return rank_name(rank) + suit_name(suit);
    }

    std::ostream& operator<<(std::ostream& os, card_t const& card) {
        os << static_cast<std::string>(card);
        return os;
    }
}