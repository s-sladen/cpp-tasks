#include <poker/human.h>

#include <iostream>
#include <algorithm>
#include <iterator>

namespace poker {
    human_player_t::human_player_t(std::string name)
            : player_t(std::move(name))
    {}

    human_player_t::~human_player_t() {
    }

    uint32_t human_player_t::bet(uint32_t your_bet, uint32_t max_bet, hand_t const& hand) {
        print_intro(hand);
        std::cout << "Your current bet: $" << your_bet
                    << ". Current max bet: $" << max_bet
                    << ". Input your next bet."
                    << std::endl;
        uint32_t bet;
        std::cin >> bet;
        return bet;
    }


    std::vector<card_t> human_player_t::exchange(hand_t & hand) {
        print_intro(hand);
        std::cout << "Enter number of cards you want to exchange" << std::endl;
        int n;
        std::cin >> n;
        std::cout << "Now enter indexes of cards you want to exchange (starting from 1)" << std::endl;
        std::vector<size_t> indexes;
        for (int i = 0; i < n; ++i) {
            size_t index;
            std::cin >> index;
            indexes.push_back(index);

        }
        std::sort(indexes.begin(), indexes.end());
        std::vector<card_t> result;
        for (int i = n - 1; i >= 0; --i) {
            result.push_back(hand.take_card(indexes[i] - 1));
        }
        return result;
    }

    void human_player_t::print_intro(hand_t const& hand) const {
        std::cout << "[" << get_name() << "] "
                << "You have $" << hand.get_cash()
                << " and following cards: ";
        auto const& cards = hand.get_cards();
        std::copy(cards.begin(), cards.end(), std::ostream_iterator<card_t>(std::cout, ", "));
        std::cout << std::endl;
    }
}