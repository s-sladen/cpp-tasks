#include <poker/rules.h>

#include <numeric>

namespace poker {
    namespace {
        bool has_straight(hand_t const& hand) {
            bool exists[card_rank::ACE + 1] = {false};
            for (card_t const& card : hand.get_cards()) {
                exists[card.get_rank()] = true;
            }
            for (size_t i = 0; i <= card_rank::ACE; ++i) {
                if (exists[i]) {
                    for (size_t j = i; j < i + 5; ++j) {
                        if (!exists[j])
                            return false;
                    }
                    return true;
                }
            }
            throw std::runtime_error("unreachable");
        }

        bool has_flush(hand_t const& hand) {
            auto const& cards = hand.get_cards();
            card_suit suit = cards.front().get_suit();
            for (card_t const& card : cards) {
                if (card.get_suit() != suit)
                    return false;
            }
            return true;
        }

        uint32_t calc_rank_mask(std::vector<std::vector<card_rank>> mults) {
            uint32_t result = 0;
            for (size_t i = 4; i > 0; --i) {
                std::sort(mults[i].begin(), mults[i].end());
                result = std::accumulate(mults[i].rbegin(), mults[i].rend(), result, [](uint32_t mask, card_rank rank) {
                    return (mask << 4) | rank;
                });
            }
            return result;
        }

        std::vector<std::vector<card_rank>> calc_mults(hand_t const& hand) {
            size_t count[card_rank::ACE + 1] = {0};
            for (card_t const& card : hand.get_cards()) {
                count[card.get_rank()]++;
            }
            std::vector<std::vector<card_rank>> mults(5);
            for (size_t i = 0; i <= card_rank::ACE; ++i) {
                mults[count[i]].push_back(static_cast<card_rank>(i));
            }
            return mults;
        }
    }

    uint32_t simple_rules::get_combination_rank(hand_t const& hand) {
        auto mults = calc_mults(hand);
        bool straight = has_straight(hand);
        bool flush = has_flush(hand);
        uint32_t rank_mask = calc_rank_mask(mults);
        if (straight && flush)
            return STRAIGHT_FLUSH | rank_mask;
        if (!mults[4].empty())
            return QUADS | rank_mask;
        if (!mults[3].empty() && !mults[2].empty())
            return FULL_HOUSE | rank_mask;
        if (flush)
            return FLUSH | rank_mask;
        if (straight)
            return STRAIGHT | rank_mask;
        if (!mults[3].empty())
            return SET | rank_mask;
        switch (mults[2].size()) {
            case 2:
                return TWO_PAIRS | rank_mask;
            case 1:
                return PAIR | rank_mask;
            default:
                return HIGHEST_CARD | rank_mask;
        }
    }

    std::string get_combination_name(uint32_t combination_rank) {
        switch (combination_rank & 0x0F000000) {
            case STRAIGHT_FLUSH:
                return "Straight flush";
            case QUADS:
                return "Quads";
            case FULL_HOUSE:
                return "Full house";
            case FLUSH:
                return "Flush";
            case STRAIGHT:
                return "Straight";
            case SET:
                return "Set";
            case TWO_PAIRS:
                return "Two pairs";
            case PAIR:
                return "Pair";
            case HIGHEST_CARD:
                return "Highest card";
        }
        throw std::runtime_error("unreachable");
    }

    uint32_t const simple_rules::SMALL_BLIND = 16;

    uint32_t const simple_rules::BIG_BLIND = 32;
}