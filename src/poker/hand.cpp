#include <poker/hand.h>

namespace poker {
    hand_t::hand_t(uint32_t cash)
            : cards()
            , cash(cash)
    {}

    hand_t::hand_t(hand_t && hand)
            : cards(std::move(hand.cards))
            , cash(hand.cash)
    {}

    hand_t& hand_t::operator=(hand_t hand) {
        std::swap(cards, hand.cards);
        std::swap(cash, cash);
        return *this;
    }

    std::vector<card_t> const& hand_t::get_cards() const {
        return cards;
    }

    card_t hand_t::take_card(size_t index) {
        card_t c = std::move(cards[index]);
        cards.erase(cards.begin() + index);
        return c;
    }

    void hand_t::add_card(card_t card) {
        cards.push_back(std::move(card));
    }

    bool hand_t::has_cards() const {
        return !cards.empty();
    }

    uint32_t hand_t::get_cash() const {
        return cash;
    }

    uint32_t hand_t::take_cash(uint32_t cash) {
        if (this->cash < cash) {
            throw std::runtime_error("Insufficient cash");
        }
        return this->cash -= cash;
    }

    uint32_t hand_t::put_cash(uint32_t cash) {
        return this->cash += cash;
    }
}